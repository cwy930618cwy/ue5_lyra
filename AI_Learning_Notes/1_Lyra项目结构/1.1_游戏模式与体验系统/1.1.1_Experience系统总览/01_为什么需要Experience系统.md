# 为什么需要 Experience 系统

## 一个场景引入

假设你要做一款射击游戏，策划说：

> "我们要三种模式：团队竞技、个人竞技、占点模式。每种模式的计分规则不同、UI 不同、甚至玩家用的角色也不同。"

**传统做法**：写三个 GameMode 子类——`ATeamGameMode`、`APersonalGameMode`、`ACapturePointGameMode`。每个子类里重复写大量相似的代码，只是细节不同。

```cpp
// ❌ 传统做法：每个模式一个类，代码大量重复
class ATeamGameMode : public AGameModeBase { /* 队伍计分 + UI ... */ };
class APersonalGameMode : public AGameModeBase { /* 个人计分 + UI ... */ };
```

**Lyra 做法**：只写一套 C++ 代码（`ALyraGameMode`），然后创建不同的**数据资产实例**来配置差异。

```cpp
// ✅ Lyra 做法：一套代码 + 不同配置 = 不同模式
// 团队竞技 → 创建一个 ExperienceDefinition 实例，勾选"Teams"插件
// 个人竞技 → 创建另一个实例，不勾"Teams"，换一套 PawnData
// 占点模式 → 再创建一个实例，加"CapturePoint"Action
```

---

## 核心思想

> **"组合优于继承"** —— 用数据资产配置（Composition）替代类继承（Inheritance）。

| 维度 | 传统做法 | Lyra 做法 |
|------|----------|-----------|
| 新增模式 | 写新的 GameMode 子类 | 复制一份 ExperienceDefinition，改配置 |
| 代码复用 | 靠继承，容易写出深层继承链 | 靠组合（Action + ActionSet），平铺直叙 |
| 谁来做 | 程序员 | 策划（编辑器里拖拽配置） |
| 运行时切换 | 不支持（类是编译时确定的） | 支持（WorldSettings 可指定不同 Experience） |
| 测试 | 每个子类单独测 | 同一套代码，只测配置组合 |

---

## 相关源码

> 📂 代码位置：`代码/` 文件夹（所有子章节共享同一份源码）

### 文件总览

| 文件 | 角色 | 说明 |
|------|------|------|
| `LyraExperienceDefinition.h` | 数据资产声明 | 定义配置表有哪些字段 |
| `LyraExperienceDefinition.cpp` | 数据资产实现 | 验证配置是否合法 |
| `LyraGameMode.h` | GameMode 声明 | 读取 Experience 决定 Pawn 类型 |
| `LyraGameMode.cpp` | GameMode 实现 | 加载流程 + Pawn 生成 |

---

### 1. `ULyraExperienceDefinition` — 配置表的"骨架"

**继承链**：`UObject → UDataAsset → UPrimaryDataAsset → ULyraExperienceDefinition`

`UPrimaryDataAsset` 意味着它是一个可以在编辑器中右键创建的独立资产（和蓝图一样有生命周期）。

#### 关键字段

| 字段 | 类型 | 说明 |
|------|------|------|
| `GameFeaturesToEnable` | `TArray<FString>` | 要启用的 GameFeature 插件列表（如 `["ShooterCore", "Teams"]`） |
| `DefaultPawnData` | `TObjectPtr<const ULyraPawnData>` | 默认 Pawn 数据（决定玩家用什么角色） |
| `Actions` | `TArray<TObjectPtr<UGameFeatureAction>>` | 内联 Action 列表（Instanced + EditInlineNew，专属本 Experience） |
| `ActionSets` | `TArray<TObjectPtr<ULyraExperienceActionSet>>` | 引用的外部 ActionSet（可复用组合包） |

> 📄 完整头文件见 `代码/LyraExperienceDefinition.h`

---

### 2. `ULyraExperienceDefinition::IsDataValid()` — 配置验证

这个函数做了两件重要的事：

1. **数据验证**：确保 Action 不为空、不允许"蓝图的蓝图"继承
2. **资源收集**（`UpdateAssetBundleData`）：告诉引擎这个 Experience 需要哪些资源

关键验证逻辑：

```cpp
// 禁止"蓝图的蓝图"继承——应用 ActionSet 组合而非蓝图继承链
if (!Action->GetClass()->IsNative())
{
    const UClass* SuperClass = Action->GetClass()->GetSuperClass();
    if ((SuperClass != nullptr) && !SuperClass->IsNative())
    {
        Context.AddError(/* "Use ActionSet composition instead of BP inheritance." */);
    }
}
```

**验证逻辑要点**：
- 递归验证：不仅检查 Action 本身，还检查 Action 内部的子对象
- 禁止"蓝图的蓝图"：如果 Action 是蓝图类，且它的父类也是蓝图类，报错。原因是要鼓励用 ActionSet 组合而非蓝图继承链

> 📄 完整实现见 `代码/LyraExperienceDefinition.cpp`

---

### 3. `ALyraGameMode` — 读取 Experience 的 GameMode

**核心职责**：
1. 游戏启动时确定使用哪个 Experience
2. Experience 加载完成后为在场玩家生成 Pawn
3. 动态决定每个玩家的 Pawn 类型（由 Experience 配置决定）

#### 关键方法

| 方法 | 职责 |
|------|------|
| `InitGame()` | 游戏初始化入口，触发 Experience 加载 |
| `HandleMatchAssignmentIfNotExpectingOne()` | 按优先级确定用哪个 Experience |
| `OnExperienceLoaded()` | Experience 加载完成回调，为在场玩家生成 Pawn |
| `GetPawnDataForController()` | 从 Experience 中查找 Controller 对应的 PawnData |
| `GetDefaultPawnClassForController_Implementation()` | 返回 Pawn 的类（由 Experience 配置决定） |

#### Experience 选择优先级

```
WorldSettings.DefaultGameplayExperience  ← 最高优先（关卡级覆盖）
        ↓ 如果没有
命令行参数 -Experience=xxx                ← 调试用
        ↓ 如果没有
项目设置 DefaultGameModeExperience        ← 兜底默认值
```

#### 关键代码：动态决定 Pawn 类型

```cpp
UClass* ALyraGameMode::GetDefaultPawnClassForController_Implementation(AController* InController)
{
    // 从 Experience 配置中查找 PawnData
    if (const ULyraPawnData* PawnData = GetPawnDataForController(InController))
    {
        if (PawnData->PawnClass)
        {
            return PawnData->PawnClass;  // 用 Experience 配置的 Pawn 类型！
        }
    }
    // 回退到父类默认行为
    return Super::GetDefaultPawnClassForController_Implementation(InController);
}
```

> 📄 完整实现见 `代码/LyraGameMode.h` 和 `代码/LyraGameMode.cpp`

---

## 完整数据流

```
┌─────────────────────────────────────────────────────────────┐
│  游戏启动                                                    │
│     │                                                       │
│     ▼                                                       │
│  InitGame()                                                 │
│     │  触发 Experience 加载                                  │
│     ▼                                                       │
│  HandleMatchAssignmentIfNotExpectingOne()                   │
│     │  按优先级确定 Experience：                             │
│     │  WorldSettings > 命令行 > 项目设置                     │
│     ▼                                                       │
│  ExperienceManagerComponent.SetCurrentExperience()          │
│     │  异步加载 GameFeature 插件 + 资源                      │
│     ▼                                                       │
│  OnExperienceLoaded()                                       │
│     │  广播委托 + 为在场玩家生成 Pawn                        │
│     ▼                                                       │
│  GetDefaultPawnClassForController()                         │
│     │  从 Experience.DefaultPawnData 获取 Pawn 类型          │
│     ▼                                                       │
│  SpawnDefaultPawnAtTransform()                              │
│     │  生成玩家角色                                          │
│     ▼                                                       │
│  玩家拥有角色！                                              │
└─────────────────────────────────────────────────────────────┘
```

---

## 一句话总结

> **"同一套代码 + 不同配置 = 不同游戏模式"**

这就是 Experience 系统的核心价值。不需要为每个游戏模式写单独的 C++ 类，只需要创建不同的 `ExperienceDefinition` 数据资产实例即可。
