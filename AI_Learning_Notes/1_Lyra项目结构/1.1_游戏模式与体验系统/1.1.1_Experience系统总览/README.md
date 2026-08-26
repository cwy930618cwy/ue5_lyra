# Experience 系统总览

## 一句话概括
Lyra 不用传统"硬编码 GameMode"的方式，而是用 **数据资产（DataAsset）描述一局游戏的玩法配置**，然后在运行时动态加载、激活。这套机制就叫 **Experience（体验）系统**。

## 为什么需要它？
传统做法：想做一个"团队竞技模式"和一个"个人竞技模式"，得写两个 GameMode 子类，代码大量重复。

Lyra 做法：
- 写一套通用的 GameMode / GameState 代码
- 用不同的 **ExperienceDefinition 数据资产** 描述差异（启用哪些插件、用什么 Pawn、执行哪些 Action）
- 同一套代码 + 不同配置 = 不同玩法模式

## 核心文件清单（10个）

| 文件 | 类型 | 干什么 |
|------|------|--------|
| `LyraExperienceDefinition.h/.cpp` | DataAsset | **体验定义**——一份配置文件，描述这局游戏要什么 |
| `LyraExperienceManagerComponent.h/.cpp` | GameStateComponent | **体验管理器**——挂在 GameState 上，负责加载/激活/卸载体验 |
| `LyraExperienceManager.h/.cpp` | EngineSubsystem | **体验管理器（引擎级）**——处理多 PIE 会话的插件引用计数 |
| `LyraExperienceActionSet.h/.cpp` | DataAsset | **Action 集合**——可复用的 Action 组合，被 Experience 引用 |
| `LyraGameMode.h/.cpp` | GameMode | **游戏模式**——玩家登录、出生、复活 |
| `LyraGameState.h/.cpp` | GameState | **游戏状态**——持有 ExperienceManagerComponent |
| `LyraWorldSettings.h/.cpp` | WorldSettings | **关卡设置**——指定这张地图默认用哪个 Experience |
| `LyraUserFacingExperienceDefinition.h/.cpp` | DataAsset | **面向玩家的体验定义**——带 UI 标题/图标/描述 |
| `LyraBotCreationComponent.h/.cpp` | GameStateComponent | **Bot 创建组件**——自动创建 AI 机器人 |
| `AsyncAction_ExperienceReady.h/.cpp` | AsyncAction | **蓝图异步动作**——等待 Experience 加载完毕 |

## 完整工作流程（时间线）

```
┌─────────────────────────────────────────────────────────────────┐
│ 1. 游戏启动 / 进入地图                                            │
│    ↓                                                             │
│ 2. ALyraGameMode::InitGame()                                     │
│    → 下一帧调用 HandleMatchAssignmentIfNotExpectingOne()          │
│    → 按优先级决定用哪个 Experience：                               │
│      OptionsString > DeveloperSettings(PIE) > 命令行              │
│      > WorldSettings > 专用服务器 > 默认值                        │
│    ↓                                                             │
│ 3. OnMatchAssignmentGiven(ExperienceId)                          │
│    → 找到 GameState 上的 ExperienceManagerComponent               │
│    → 调用 SetCurrentExperience(ExperienceId)                     │
│    ↓                                                             │
│ 4. ExperienceManagerComponent::StartExperienceLoad()             │
│    状态: Unloaded → Loading                                      │
│    → 预加载 Experience 及其 ActionSets 的资源 Bundle              │
│    ↓                                                             │
│ 5. OnExperienceLoadComplete()                                    │
│    状态: Loading → LoadingGameFeatures                           │
│    → 收集所有需要的 GameFeature 插件 URL                          │
│    → 逐个调用 LoadAndActivateGameFeaturePlugin()                 │
│    ↓                                                             │
│ 6. OnGameFeaturePluginLoadComplete() (每个插件完成回调)           │
│    → 计数 -1，全部完成后进入下一步                                │
│    ↓                                                             │
│ 7. OnExperienceFullLoadCompleted()                               │
│    状态: LoadingGameFeatures → ExecutingActions → Loaded         │
│    → 执行所有 Action（OnGameFeatureRegistering/Loading/Activating）│
│    → 广播 OnExperienceLoaded 委托（高优先级→普通→低优先级）       │
│    ↓                                                             │
│ 8. ALyraGameMode::OnExperienceLoaded()                           │
│    → 为已经在场的玩家生成 Pawn                                    │
│    → 后续新进来的玩家由 HandleStartingNewPlayer 处理              │
└─────────────────────────────────────────────────────────────────┘
```

## Experience 加载状态机

```
Unloaded ──→ Loading ──→ LoadingGameFeatures ──→ ExecutingActions ──→ Loaded
                                                              ↑            │
                                                              │            ↓
                                                    (可选chaos延迟)   Deactivating ──→ Unloaded
```

枚举定义（`LyraExperienceManagerComponent.h`）：
```cpp
enum class ELyraExperienceLoadState
{
    Unloaded,           // 未加载
    Loading,            // 正在加载资源
    LoadingGameFeatures,// 正在加载 GameFeature 插件
    LoadingChaosTestingDelay, // 混沌测试延迟（调试用）
    ExecutingActions,   // 正在执行 Action
    Loaded,             // 完全加载完毕
    Deactivating        // 正在卸载
};
```

## 谁需要什么？（依赖关系）

```
ALyraGameMode
  ├── 依赖 ALyraGameState（构造函数里指定 GameStateClass）
  ├── 依赖 ULyraExperienceManagerComponent（从 GameState 上找）
  ├── 依赖 ULyraExperienceDefinition（通过 AssetManager 加载）
  └── 依赖 ULyraPawnData（从 Experience 里拿 DefaultPawnData）

ALyraGameState
  ├── 持有 ULyraExperienceManagerComponent（子对象）
  └── 实现 IAbilitySystemInterface（GameState 级别的全局 ASC）

ULyraExperienceDefinition（DataAsset，策划在编辑器里配）
  ├── TArray<FString> GameFeaturesToEnable    ← 要启用的插件名列表
  ├── TObjectPtr<ULyraPawnData> DefaultPawnData ← 默认 Pawn 数据
  ├── TArray<UGameFeatureAction*> Actions      ← 要执行的 Action 列表
  └── TArray<ULyraExperienceActionSet*> ActionSets ← 引用的 Action 集合
```

## 对你项目的启发
你的 `ue5_lyra` 项目目前不需要完整的 Experience 系统（那是多人在线游戏才需要的），但你可以借鉴：
1. **数据驱动思想**：用 DataAsset 配置玩法参数，而不是硬编码
2. **GameFeature 插件机制**：功能模块化，按需启用
3. **Action 生命周期**：Registering → Loading → Activating → Deactivating → Unregistering

---

## 本目录文件导航

| # | 文件/文件夹 | 内容 |
|---|-------------|------|
| 01 | [为什么需要Experience系统](01_为什么需要Experience系统/README.md) | 传统 vs Lyra 对比 + **相关源码**（含注释） |
| 02 | [核心文件清单](02_核心文件清单.md) | 10个类按职责分类 |
| 03 | [完整工作流程时间线](03_完整工作流程时间线.md) | 8步流程逐条详解 |
| 04 | [加载状态机](04_加载状态机.md) | 7个状态 + Chaos Testing |
| 05 | [依赖关系图](05_依赖关系图.md) | 类依赖 + 生命周期 + 数据流 |

---

> 📂 相关文件路径：`Source/LyraGame/GameModes/`
