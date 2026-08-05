# UE5 GAS + 战斗系统教学经验库

> 本文件沉淀用户在学习 UE5 GAS（Gameplay Ability System）+ 战斗系统过程中遇到的坑点和最佳实践。
> 追加格式见 [README.md](./README.md)。

---

## 2026-04-25  Enhanced Input — IA 加载 / 按键绑定 / 触发时机

**场景**：在 `ABattleCharacter` 里新增两个测试 IA（`IA_TestDamage` H键、`IA_TestHeal` J键），绑定到成员函数 `TestTakeDamage()` / `TestHeal()`。

### 坑点 1：IA 资产路径写错，`Succeeded()` 永远 false
- **现象**：编译通过、游戏启动正常，但按 H/J 键完全没反应，UE_LOG 也不打印
- **根因**：`ConstructorHelpers::FObjectFinder` 的路径字符串哪怕错一个字母（大小写、下划线、复数 s），就会静默失败
- **正确做法**：
  1. 在 UE 编辑器里右键 IA 资产 → **Copy Reference** 拿到准确路径
  2. 路径格式必须是 `/Game/xxx/IA_Name.IA_Name`（**后半段资产名要重复一次**）
  3. 写完后可以临时加 `UE_LOG(LogTemp, Warning, TEXT("Finder Succeeded: %d"), Finder.Succeeded());` 验证

### 坑点 2：`ETriggerEvent::Triggered` vs `Started` 混淆
- **现象**：H 键一按血瞬间扣光 / J 键一按直接满血
- **根因**：
  - `Triggered` = **每帧**都触发（按住会连续调用）→ 适合移动、持续输入
  - `Started` = 按下那**一瞬间只触发 1 次** → 适合单次动作（攻击、跳跃、测试扣血）
  - `Completed` = 松开时触发 1 次 → 适合 StopJump / StopSprint
- **最佳实践**：单次动作一律用 `Started`，持续输入用 `Triggered`

### 坑点 3：`BindAction` 第 4 个参数的函数指针写法
- **正确**：`&ABattleCharacter::TestTakeDamage`
- **常见错误**：
  - ❌ `&BattleCharacter::TestTakeDamage`（漏了 `A` 前缀）
  - ❌ `&ABattleCharacter::TestTakeDamage()`（多了括号，这是**函数指针**不是函数调用）
  - ❌ `this->TestTakeDamage`（C++ 成员函数指针语法不支持 `this->`）
  - ❌ `TestTakeDamageAction`（这是 IA 变量，不是函数指针）
- **记忆口诀**："取地址符 `&` + 类名 `::` + 函数名（不带括号）"

### 坑点 4：函数名和 IA 变量名容易混用
- IA 变量：`TestTakeDamageAction`（**带 Action 后缀**）
- 回调函数：`TestTakeDamage`（**不带 Action 后缀**）
- BindAction 里：第 1 个参数是 IA（带 Action），第 4 个参数是函数指针（不带 Action）

**关联文件**：[BattleCharacter.cpp](../Source/ue5_lyra/Character/BattleCharacter.cpp) / [BattleCharacter.h](../Source/ue5_lyra/Character/BattleCharacter.h)
**相关 API**：`UEnhancedInputComponent::BindAction` / `ETriggerEvent` / `ConstructorHelpers::FObjectFinder<UInputAction>`

---

## 2026-04-25  GAS 初始化 —— ASC + AttributeSet 的创建顺序

**场景**：在 `ABattleCharacter` 构造函数里创建 `UAbilitySystemComponent` 和 `UHealthSet`。

### 关键点
1. **ASC 必须先创建，AttributeSet 后创建**：AttributeSet 会自动挂到"最后一个被创建的 ASC"上
2. **必须用 `CreateDefaultSubobject`**，不能用 `NewObject`（构造函数阶段 NewObject 会崩）
3. **复制模式**：多人游戏里要设 `SetReplicationMode(EGameplayEffectReplicationMode::Mixed)`（玩家用 Mixed，AI 用 Minimal）
4. **属性初始化必须放到 `BeginPlay`，不能放构造函数**：构造函数阶段 AttributeSet 还没完全 ready

### 正确代码模板
```cpp
// 构造函数里
AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
AbilitySystemComponent->SetIsReplicated(true);
AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
HealthSet = CreateDefaultSubobject<UHealthSet>(TEXT("HealthSet"));

// BeginPlay 里初始化数值
if (AbilitySystemComponent && HealthSet)
{
    HealthSet->InitMaxHealth(100.0f);  // 先设上限
    HealthSet->InitHealth(100.0f);      // 再设当前值
}
```

**关联文件**：[BattleCharacter.cpp](../Source/ue5_lyra/Character/BattleCharacter.cpp) / [HealthSet.h](../Source/ue5_lyra/Attributes/HealthSet/HealthSet.h)
**相关 API**：`UAbilitySystemComponent` / `UAttributeSet` / `InitMaxHealth` / `InitHealth`

---

## 2026-04-25  通过 ASC 修改属性 —— ApplyModToAttribute

**场景**：实现 `TakeDamage(float)` / `Heal(float)` 扣血/加血函数。

### 正确调用方式
```cpp
AbilitySystemComponent->ApplyModToAttribute(
    UHealthSet::GetHealthAttribute(),   // 属性（用【类名::静态函数】调用）
    EGameplayModOp::Additive,           // 操作：加法
    -Amount                             // 数值（扣血传负数，加血传正数）
);
```

### 坑点
1. `GetHealthAttribute()` 必须用 `UHealthSet::GetHealthAttribute()` 调用，**不能用 `HealthSet->GetHealthAttribute()`**（虽然编译可能通过，但这是类级静态函数，语义上应该用类名）
2. `EGameplayModOp` 可选值：`Additive`（加）/ `Multiplicitive`（乘）/ `Division`（除）/ `Override`（覆盖）
3. 扣血传负数，不要自己算新值再 `Override`，否则触发不了 `PostGameplayEffectExecute` 的钳制逻辑

**关联文件**：[BattleCharacter.cpp](../Source/ue5_lyra/Character/BattleCharacter.cpp)（`TakeDamage` / `Heal` 函数）
**相关 API**：`UAbilitySystemComponent::ApplyModToAttribute` / `EGameplayModOp` / `FGameplayAttribute`

---

## 2026-04-25  Test 回调函数 vs 核心业务函数 —— 薄封装模式

**场景**：给测试按键（H/J）绑定回调时，需要实现 `TestTakeDamage()` / `TestHeal()`。

### 关键决策：不要在回调里重新写扣血/加血逻辑
- **反模式**：`TestTakeDamage()` 里直接写 `AbilitySystemComponent->ApplyModToAttribute(...)`
- **正确做法**：`TestTakeDamage()` 只做一件事 —— 调用 `TakeDamage(10.0f)`
- **理由**：
  1. 核心业务逻辑（扣血/加血）只在一处维护，未来改钳制、加日志、加事件都集中改一处
  2. 测试回调只是"按键 → 业务函数"的薄封装层，保持纯净
  3. 符合单一职责原则

### 正确代码模板
```cpp
// 核心业务
void ABattleCharacter::TakeDamage(float Amount) { /* ApplyModToAttribute ... */ }

// 测试回调（薄封装）
void ABattleCharacter::TestTakeDamage() { TakeDamage(10.0f); }
```

### ⚠️ 致命陷阱：函数递归自调
- ❌ `void TestTakeDamage() { TestTakeDamage(10.0f); }` — 这不是递归，是编译错误（签名不同无法重载成不带参版本调用带参版本；即使签名相同也会死循环）
- ✅ `void TestTakeDamage() { TakeDamage(10.0f); }` — 调用的是**另一个**函数 `TakeDamage`

**关联文件**：[BattleCharacter.cpp](../Source/ue5_lyra/Character/BattleCharacter.cpp)（`TestTakeDamage` / `TestHeal`）

---

## 2026-04-25  P2-T2.1 阶段完整测试检查清单

**场景**：P2 → T2.1（Enhanced Input + GAS 血量系统 ApplyModToAttribute 版）完工后的端到端测试流程。

### 测试前必做
1. **关闭 UE 编辑器**（防止 C++ 文件被占用导致链接失败 `LNK1168`）
2. 在 VS/Rider 里编译（Ctrl+Shift+B），期望 0 errors

### PIE 测试用例
| 操作 | 预期日志（Output Log 过滤 LogTemp） | 说明 |
|---|---|---|
| 按 H | `[扣血] -10.0 → 当前：90.0 / 100.0` | 单次触发 |
| 连按 H 11 次 | 最后一次显示 `当前：0.0 / 100.0`（不会负数） | `PostGameplayEffectExecute` 钳制生效 |
| 按 J | `[加血] +10.0 → 当前：X.0 / 100.0` | |
| 血满按 J | `当前：100.0 / 100.0`（不会超过 Max） | 上限钳制生效 |

### 如果测试失败，排查矩阵
- **按键无反应、无日志** → IA 路径错 / IMC 没绑 H/J / BeginPlay 没 AddMappingContext
- **有日志但血量不变** → ASC 或 HealthSet 为 null / 构造函数没 CreateDefaultSubobject
- **血量变负数 / 超 100** → HealthSet.cpp 的 `PostGameplayEffectExecute` 钳制没写或写错
- **编译报 unresolved external** → .h 声明了函数但 .cpp 没实现 / 忘加 `ABattleCharacter::` 作用域

**关联文件**：[BattleCharacter.cpp](../Source/ue5_lyra/Character/BattleCharacter.cpp) / [HealthSet.cpp](../Source/ue5_lyra/Attributes/HealthSet/HealthSet.cpp)

---

## 2026-04-25  Gameplay Tag —— 为什么要用 Tag 而不是字符串

**场景**：教学 GE_Damage 时，用户问"注册 `Data.Damage` 这个 Tag 到底是为什么？"

### Tag 的本质
- **Tag 不是资产**（.uasset），是写在 `Config/DefaultGameplayTags.ini` 里的一条配置
- 注册后 UE 分配一个全局唯一 ID（`FGameplayTag` 底层是 uint32）
- 用 `.` 分隔层级，天然树状结构（`Data.Damage` 是 `Data` 的子节点）

### 为什么不用裸字符串
| 问题 | 裸字符串 `"Data.Damage"` | Gameplay Tag |
|---|---|---|
| 拼写错误 | 编译通过，运行时静默失效 | `RequestGameplayTag` 找不到会报错 |
| 改名 | 全局搜索替换，易漏 | 管理器里改名自动同步所有引用 |
| 层级关系 | 无 | 父 Tag 匹配所有子 Tag |
| 蓝图填写 | 手打 | 下拉菜单补全 |
| 网络传输 | 字符串 | 整数 ID（压缩） |

### 层级匹配规则（高频考点）
```cpp
Tag.MatchesTag(ParentTag)       // 父 Tag 匹配所有子 Tag（"Ability.Attack" 匹配 "Ability.Attack.Melee"）
Tag.MatchesTagExact(OtherTag)   // 只匹配自己，不匹配子
```

### 典型用法：Tag 作为"参数传递插口"
这是 `Data.Damage` 的核心用途——连通 C++ 和 GE 蓝图资产：

**C++ 喂数据**：
```cpp
SpecHandle.Data->SetSetByCallerMagnitude(
    FGameplayTag::RequestGameplayTag("Data.Damage"), 10.0f);
```

**GE 资产读数据**（细节面板配置）：
```
Modifier:
  Magnitude Calculation Type = Set By Caller
  Data Tag = Data.Damage   ← 两边用同一个 Tag 对接
```

### 数据驱动的价值
同一个 `GE_Damage` 资产通过 SetByCaller 能复用：
- 普攻喂 10、暴击喂 30、技能喂 80、DoT 喂 5
- 策划在蓝图里配"效果框架"（扣什么属性/加还是乘），C++ 运行时填实际数值

**关联文件**：`Config/DefaultGameplayTags.ini`
**相关 API**：`FGameplayTag::RequestGameplayTag` / `FGameplayTagContainer::HasTag` / `MatchesTag` / `MatchesTagExact` / `SetSetByCallerMagnitude`

---

## 2026-04-25  新建 Gameplay Tag 后 DefaultGameplayTags.ini 里没有 GameplayTagList

**场景**：用户按教程新建了 `Data.Damage`，但打开 `Config/DefaultGameplayTags.ini` 只看到 `[/Script/GameplayTags.GameplayTagsSettings]` 段（Tag 系统全局设置），没有期望的 `[/Script/GameplayTags.GameplayTagsList]` 段和 `+GameplayTagList=(Tag="...")` 行。

### 根因（三种可能）
1. **Tag 存到了其他文件**：新建 Tag 时【源 Source】字段可能默认指向 `Config/Tags/DefaultGameplayTags.ini`（Tags 子目录），而不是 `Config/DefaultGameplayTags.ini`（项目根 Config）。两个路径都叫同名文件，极易混淆。
2. **Tag 存到了 DataTable 资产**：Source 可能是某个 `.uasset`，此时 .ini 里完全不会出现该 Tag。
3. **UE 还没落盘**：项目设置修改后有时滞留内存，需点保存按钮或重启编辑器才刷磁盘。

### 正确排查顺序
1. **验证 Tag 是否注册成功**：打开【编辑 → 项目设置 → GameplayTags → 游戏性标签列表】，搜索 `Data.Damage`
   - 搜到了 → Tag OK，只是存在别处
   - 搜不到 → 当初的【添加新标签】操作没提交成功，重做
2. **查看 Tag 的 Source 字段**：在列表里点开该 Tag，Source 会指示真实存储位置
3. **物理检查**：`e:\ue5\ue5_lyra\Config\` 和 `e:\ue5\ue5_lyra\Config\Tags\` 两个目录下的所有 .ini 都 grep 一遍 `Data.Damage`

### 结论
**"DefaultGameplayTags.ini 里找不到新建的 Tag"≠ Tag 没注册成功**，先去项目设置面板确认真实注册状态，再按 Source 字段定位真实存储文件。

### 用户心智模型修正
不要让用户直接打开 .ini 文件验证 Tag 是否注册，应该让他去项目设置面板的 Tag 列表 UI 里搜索验证（这才是 UE 的 single source of truth）。

**关联文件**：`Config/DefaultGameplayTags.ini` / `Config/Tags/*.ini`
**相关术语**：Source (来源) / GameplayTagList / GameplayTagsSettings

---

## 2026-04-26  【P0 教学工作流铁律】讲代码前必须自己查文件当前状态

**场景**：给用户讲 GAS 扣血 C++ 改造，教程写完后 review 时才想起"顺便检查 Build.cs 有没有加依赖"、"include 区有没有包含 AbilitySystemBlueprintLibrary.h"，让用户自己去开文件核对。结果用户实际上早就加好了，白让他跑一趟。

### 坑点
- 教程给步骤 A B C，**漏了步骤 D**（比如必要的 include），等用户写完才补课
- 让用户"请检查 XXX 文件有没有 YYY"，而**我有工具可以自己 read_file 直接看**
- 用户的反馈："这个你教程为什么没有？" / "自己找文件存不存在 不要叫我找"

### 根因
- 出教程前没有先扫一遍相关文件（当前状态 / 已有 include / 已有依赖 / 已有函数），导致教学步骤不完整
- review 阶段把"验证当前状态"的工作推给用户，而不是自己用工具查

### 正确做法（永久生效）
1. **每次出教程前，先 read_file 当前所有相关文件**：.h / .cpp / Build.cs / .ini / 关联的资产配置文件
2. **教程一次性把所有依赖全部列出**：include / 模块依赖 / Tag 注册 / 资产路径，一个都不能漏
3. **review 阶段要自查的内容，全部自己用工具查**，不要说"请你检查 XXX 有没有 YYY"
4. **唯一允许让用户检查的**：只能用 UE 编辑器 UI 操作的东西（GE 蓝图 Modifier 配置、项目设置里的 Tag 列表、IMC 里的 IA 绑定键位等）——这些工具看不到
5. **如果教程出错了，立刻认错 + 自查当前状态 + 告知用户"不用你动手了"**，不要让用户再跑一次

### 口诀
> 「文件能自己看的，绝不叫用户看；编辑器里才能点的，才让用户动。」

**关联场景**：出教程 / review / 排查编译报错 / 检查依赖
**违反后果**：用户会明确斥责"为什么这个你教程没有？" / "不要叫我找"

---

## 2026-04-26  【P0 教学工作流铁律】新功能教学必须按"先建资产 → 再敲 C++"的顺序

**场景**：教用户把 `Heal()` 从 `ApplyModToAttribute` 直改改造为走 GE 流程时，开头就甩了一大段 C++ 加载 `GE_Heal` 的代码，但 `GE_Heal` 蓝图还没创建、`Data.Heal` Tag 还没注册，导致用户一脸懵："这是什么东西？我只有 GE_Damage 啊"。

### 坑点
- 教 `GE_Damage` 时明明是**先注册 Tag → 建 GE 蓝图 → 最后敲 C++**，顺序清晰
- 教 `GE_Heal` 时却**倒着来**：先甩 C++ 加载代码 → 最后才说"哦对了要去创建 GE 蓝图"
- 用户的反馈："那你不应该先叫我建资产？跟 GE_Damage 教程一个顺序？"

### 根因
- 出教程时按"**C++ 代码结构顺序**"组织步骤（先字段 → 后函数 → 后资产），而不是"**依赖关系顺序**"（先有资产 → C++ 才能引用）
- 忽略了"教学一致性"：同类型功能的教学流程必须统一，不能这次先建资产、下次先写代码

### 正确做法（永久生效）
**凡是"创建新 GE / 新 GA / 新 AttributeSet / 新 Tag"类功能，强制按以下顺序教学**：

```
Step 1 【UE 编辑器】注册 Gameplay Tag（如果需要）
Step 2 【UE 编辑器】创建蓝图资产（GE / GA / DataAsset）
Step 3 【UE 编辑器】配置资产细节面板（Modifier / Duration / Set By Caller 等）
Step 4 【UE 编辑器】编译 + 保存蓝图
Step 5 【VSCode】  .h 加字段声明
Step 6 【VSCode】  .cpp 构造函数加载资产（FClassFinder）
Step 7 【VSCode】  .cpp 实现/替换业务函数（使用加载好的资产）
Step 8 【UE 编辑器】Live Coding 热重载 → 测试
```

### 为什么这个顺序最合理
1. **资产先行**：C++ 里的 `FClassFinder("/Game/.../GE_Heal")` 依赖资产已存在，否则运行时会报"加载失败"
2. **心智连续**：和上次教 `GE_Damage` 的顺序完全一致，用户肌肉记忆能复用
3. **即时验证**：每完成一步都可以立刻看到效果（Tag 进 .ini / 蓝图出现在内容浏览器 / C++ 打日志）
4. **降低认知负担**：先在 UE 编辑器里"看得见摸得着"做完资产配置，再回 VSCode 敲抽象的 C++ 引用

### 反模式
- ❌ 先甩 C++ 代码 → "这段代码引用了一个 GE_Heal 蓝图" → "哦对了你还没有这个蓝图，现在去建"
- ❌ 同类型功能教学顺序前后不一致（这次先资产、下次先代码）

### 口诀
> 「先有资产、再写代码；教学顺序、前后统一。」

**关联场景**：教 GE_Damage / GE_Heal / GA / AttributeSet 初始化配置等所有"C++ 依赖蓝图资产"的场景
**违反后果**：用户会问"这是什么东西我没见过" / "你顺序怎么跟上次不一样"

---

## 2026-04-26  【P0 教学工作流铁律】按教程走，一步做完立刻进下一步，不要停下来问用户

**场景**：教学过程中每完成一步（Step N review 通过），我总在末尾写"跟我说'继续'就进 Step N+1"，等用户确认。用户明确反馈："P0 经验 按教程走 做完了就下一步 不要问我"。

### 坑点
- review 通过后末尾挂一句"说'继续'进下一步" / "要进 Step N+1 吗？" / "确认后我们继续"
- 这种"礼貌性确认"在教学连续节奏里是**纯粹的干扰和冗余**
- 用户的反馈："不要问我" / "做完了就下一步"

### 根因
- 把"征询用户意见"的通用礼貌准则错误套用到**连续教学场景**
- 没区分两种场景：
  - A. **多个方案选择**（需要用户决策）→ 必须停下来问
  - B. **线性教程推进**（下一步是唯一路径）→ 不要问，直接走

### 正确做法（永久生效）
1. **review 通过后立刻出下一步教程**，不要在末尾挂"说'继续'就进 Step N+1"
2. **下一步的完整教程内容直接写在 review 回答的后半段**，用分隔线隔开：
   ```
   ## Review 结果 ✅ [...]
   
   ---
   
   # 📝 Step N+1：XXX
   [完整教程内容]
   ```
3. **只有以下两种情况才停下来问用户**：
   - 用户需要去 UE 编辑器做操作（让用户自己操作完回来说"好了"）
   - 真的遇到多个方案分叉需要用户决策
4. **不要问的典型话术（禁用）**：
   - ❌ "跟我说'继续'就进下一步"
   - ❌ "要进 Step N+1 吗？"
   - ❌ "确认 OK 后我们继续"
   - ❌ "你说'下一步'我就开始"

### 正确节奏示例
```
【用户】af 加好了
【我】  ✅ Review：字段类型 / 位置 / 分号都对
        ---
        # Step 4：在 .cpp 构造函数里加载 GE_Heal
        【完整代码 + 讲解 + 自检清单】
        ---
        敲完说一声，我 review 后进 Step 5 💪
```

### 反模式
```
【用户】af 加好了
【我】  ✅ Review 通过
        下一步：Step 4 在 .cpp 里加载 GE_Heal
        跟我说"继续"就进 Step 4 💪   ← 这句完全多余！应该直接给出 Step 4 完整教程
```

### 口诀
> 「线性教程不问路，review 完就发下一步。」

**关联场景**：GAS / 战斗系统 / UE C++ 所有连续步骤教学
**违反后果**：用户会斥责"不要问我" / "做完了就下一步"

---

<!-- 新经验追加到这里 ↓↓↓ -->
