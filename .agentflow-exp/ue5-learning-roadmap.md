# 🗺️ UE5 Lyra 实战学习 —— 总路线图（Single Source of Truth）

> 本文件是本项目教学的**唯一权威进度源**。
> AI 每次回答前必读本文件，**不再问用户"走哪条路"**。
> 用户每次推进，AI 负责更新本文件的"当前位置"和"下一步"。

---

## 🎯 总目标（AgentFlow 对应：`goal_19d9b74aa1c_21fe228a`）

以「先会用、再深入」为核心理念，从可见的游戏功能出发（人物走动→攻击→技能→关卡），
逐步深入 Lyra 架构底层原理。每个阶段都产出可运行的游戏功能，在实践中理解架构设计。

- 项目路径：`e:\ue5\ue5_lyra`
- 参考项目：`E:\ue5\LyraStarterGame`
- 教学模式：引导用户亲手敲代码，AI 不代写（见 README 三铁律）
- 环境：VS2022 17.14 + Rider 2025.1.6 + UE 5.5.4 + VS Code

---

## 📊 六阶段总览

| 阶段 | 主题 | 状态 | AgentFlow Goal ID |
|---|---|---|---|
| **P1** | 人物走动 —— 让角色跑起来 | ✅ 完成 | `goal_19d9b75371a_105178d3` |
| **P2** | 攻击与伤害 —— 让角色能打 | 🔄 **进行中** | `goal_19d9b7537c8_29f692dc` |
| P3 | 技能系统 —— 让角色更强 | ⏸️ 未开始 | `goal_19d9b7537de_bb9d2e49` |
| P4 | 关卡与游戏模式 —— 让游戏有内容 | ⏸️ 未开始 | `goal_19d9b7537f1_f41deca5` |
| P5 | UI 与游戏体验 —— 让游戏好玩 | ⏸️ 未开始 | `goal_19d9b7537ff_69a637e0` |
| P6 | 深入底层架构 —— 理解为什么这样设计 | ⏸️ 未开始 | `goal_19d9b753813_4dcf4f96` |

---

## 🔍 P2 详细分解（当前阶段）

**目标**：实现武器装备 + 射击/近战攻击 + 伤害判定 + 生命值系统。
**对标 Lyra**：`LyraEquipmentDefinition` + `LyraWeaponInstance` + `LyraGameplayAbility_RangedWeapon` + `LyraHealthComponent` + `HealthSet/CombatSet`。
**产出**：角色能装备武器、攻击敌人、敌人掉血、角色也能被打死。

### P2 子任务清单

| 子任务 | 标题 | 状态 | AgentFlow Task ID |
|---|---|---|---|
| **T2.1** | **生命值系统（HealthComponent + AttributeSet）** | 🔄 **进行中（约 60%）** | `task_19d9b782c34_43a778f0` |
| T2.2 | 装备系统基础（EquipmentDefinition/Instance/Manager） | ⏸️ | `task_19d9b782c34_2fc5ac3c` |
| T2.3 | 射击技能（GA_Fire + 射线检测 + GE_Damage） | ⏸️ | `task_19d9b782c34_a9a14f85` |
| T2.4 | 敌人 AI（EnemyCharacter + 受击反馈） | ⏸️ | `task_19d9b782c34_a0f229e5` |
| T2.5 | 完整伤害链路（CombatSet + 攻防计算 + 死亡流程） | ⏸️ | `task_19d9b782c34_b9fe134d` |

---

## 🎯 T2.1 详细分解（当前任务）

### T2.1 验收清单

| # | 验收点 | 状态 |
|---|---|---|
| 1 | Build.cs 加 GameplayAbilities/GameplayTags/GameplayTasks 依赖 | ✅ |
| 2 | 创建 `UHealthSet`（继承 UAttributeSet + ATTRIBUTE_ACCESSORS 宏，含 Health/MaxHealth） | ✅ |
| 3 | BattleCharacter 挂 `AbilitySystemComponent`（CreateDefaultSubobject） | ✅ |
| 4 | BattleCharacter 挂 `HealthSet` | ✅ |
| 5 | BeginPlay 初始化属性（InitMaxHealth → InitHealth） | ✅ |
| 6 | PostGameplayEffectExecute 钳制 `0 ≤ Health ≤ MaxHealth` | ✅ |
| 7 | Debug 能打印血量（H/J 键 ApplyModToAttribute 测试通过） | ✅ |
| 8 | **把 `TakeDamage` 从 ApplyModToAttribute 升级为 GameplayEffect 流程** | 🔄 **当前小节** |
| 9 | 创建 `UMyHealthComponent` 监听属性变化（对标 `LyraHealthComponent`） | ⏸️ |

---

## 🚀 当前小节：T2.1-第8项 —— GameplayEffect（GE）升级

### 🧠 为什么要升级？

`ApplyModToAttribute` 是 Epic 官方标注的"**绕过 GAS 流程**"的快捷方式（源码注释：*bypassing any tags or other GameplayEffects*）。
只适合初始化 / 作弊指令 / 单元测试，**正式战斗必须用 GameplayEffect**。
T2.3 射击技能需要 `ApplyGameplayEffectToTarget` —— 必须先会造 GE。

### 📐 升级后的最终形态

```cpp
void ABattleCharacter::TakeDamage(float Amount)
{
    FGameplayEffectContextHandle Context = AbilitySystemComponent->MakeEffectContext();
    FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(
        DamageEffectClass, 1.0f, Context);

    if (SpecHandle.IsValid())
    {
        SpecHandle.Data->SetSetByCallerMagnitude(
            FGameplayTag::RequestGameplayTag(FName("Data.Damage")), Amount);
        AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data);
    }
}
```

### 🪜 拆分的 5 小步

| 小步 | 做什么 | 涉及文件 | 状态 |
|---|---|---|---|
| ① | UE 编辑器建 `Data.Damage` GameplayTag + 建 `GE_Damage` 蓝图资产 | 纯资产操作 | 🔄 **进行中** |
| ② | `BattleCharacter.h` 声明 `TSubclassOf<UGameplayEffect> DamageEffectClass` | .h | ⏸️ |
| ③ | 构造函数里加载 `GE_Damage` 蓝图类 | .cpp | ⏸️ |
| ④ | 改写 `TakeDamage()` 用 GE 流程（`MakeOutgoingSpec` → `SetSetByCallerMagnitude` → `ApplyGameplayEffectSpecToSelf`） | .cpp | ⏸️ |
| ⑤ | `Build.cs` 加 `GameplayTags` 依赖（如果还没加）+ 测试 H 键 | Build.cs + PIE | ⏸️ |

---

## 📋 第 ① 小步：UE 编辑器操作（纯 UI，不涉及代码）

### Part 1：注册 GameplayTag `Data.Damage`

```
Edit → Project Settings
→ 左侧找 Project 分类下的 GameplayTags
→ 右侧 Gameplay Tag List → Add New Gameplay Tag（+ 号）

Name     : Data.Damage
Comment  : 扣血数值载荷Tag，GE通过SetByCaller读取扣血量
Source   : 默认 DefaultGameplayTags.ini
```

### Part 2：创建 `GE_Damage` 蓝图资产

```
Content Browser → /Game/MyResource/
→ 新建文件夹 GameplayEffects
→ 进入 GameplayEffects
→ 右键空白 → Blueprint Class
→ 顶部搜索 "GameplayEffect" → 选中 → Select
→ 命名 GE_Damage
```

**双击打开 GE_Damage，填写：**

| 字段 | 值 |
|---|---|
| Duration Policy | **Instant** |
| Modifiers | 点 + 号新增 1 条 |
| └─ Attribute | `HealthSet.Health` |
| └─ Modifier Op | `Add` |
| └─ Magnitude Calculation Type | `Set By Caller` |
| └─ Set By Caller → Data Tag | `Data.Damage` |

---

## ⚠️ 常见踩坑（预警）

1. Attribute 下拉选不到 `HealthSet.Health` → 项目没编译或 UE 没重启
2. Set By Caller 的 Data Tag 下拉没有 `Data.Damage` → Part 1 的 Tag 没注册成功
3. Modifier Op 选成 `Override` → 会把血量覆盖成 10 而不是扣 10
4. Duration Policy 选错成 Duration → 变成"持续 N 秒" 而不是"立即生效一次"

---

## 📝 推进规则

1. 用户每完成一小步，AI 更新本文件的"状态"列（🔄 → ✅）
2. 当前小节完成后，AI 把"🚀 当前小节"指针**推进到下一小节**
3. 每大节（如 T2.1）完成后，在 `ue5-gas-teaching.md` 追加本节的核心经验
4. 进入下一大任务（如 T2.2）前，先读取对应的 AgentFlow Task 描述刷新本文件

---

## 🔗 关联文档

- [README.md](./README.md) —— 经验库工作协议
- [ue5-gas-teaching.md](./ue5-gas-teaching.md) —— GAS 技术点坑点沉淀
- AgentFlow 总目标：`goal_19d9b74aa1c_21fe228a`

<!-- 推进时更新下方锚点 -->
<!-- LAST_UPDATED: 2026-04-25 -->
<!-- CURRENT_POSITION: P2 → T2.1 → 第8项GE升级 → 第①小步UE编辑器操作 -->
