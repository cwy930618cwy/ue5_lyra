# 为什么需要 Experience 系统？

## 传统做法的问题

假设你要做两个游戏模式：**团队竞技**（有队伍、占点）和 **个人竞技**（无队伍、自由混战）。

### ❌ 传统做法：写两个 GameMode 子类

```cpp
// 问题：大量重复代码，只有少数配置不同
class ATeamGameMode : public AGameMode { /* 90% 代码和下面一样 */ };
class APersonalGameMode : public AGameMode { /* 90% 代码和上面一样 */ };
```

**痛点：**
- 每新增一个模式就要写一个新类
- 策划改个 Pawn 类型也要找程序员改代码
- 测试时间翻倍（每个类都要测）

---

## Lyra 的解决方案：数据驱动

**核心理念：** 把"玩法差异"从代码中抽出来，放到**数据资产（DataAsset）**里。

- 程序只写**一套** GameMode 逻辑
- 策划通过创建/修改 DataAsset 就能调整玩法
- 同一套 C++ 代码 + 不同的 ExperienceDefinition 配置 = 不同游戏模式

### ✅ Lyra 做法：一份配置 = 一种玩法

| 配置项 | 团队竞技 | 个人竞技 |
|--------|----------|----------|
| `GameFeaturesToEnable` | `["ShooterCore", "Teams"]` | `["ShooterCore"]` |
| `DefaultPawnData` | 标准士兵 | 快速移动兵 |
| `Actions` | 队伍 UI + 占点计分 | 个人计分 UI |

---

## 相关源码

> 📂 代码位置：`代码/` 文件夹

### 1. `LyraExperienceDefinition.h` — 4 个字段的定义

| 行号 | 内容 | 说明 |
|------|------|------|
| 第 36 行 | `UCLASS(BlueprintType, Const)` | 蓝图可见 + 运行时不可修改 |
| 第 65~66 行 | `GameFeaturesToEnable` | ① 要启用的 GameFeature 插件列表 |
| 第 74~75 行 | `DefaultPawnData` | ② 默认 Pawn 数据（玩家用什么角色） |
| 第 84~85 行 | `Actions` | ③ 直接定义的 Action 列表（内联） |
| 第 93~94 行 | `ActionSets` | ④ 引用的 ActionSet（可复用组合） |

### 2. `LyraExperienceDefinition.cpp` — 数据验证

| 行号 | 内容 | 说明 |
|------|------|------|
| 第 37~87 行 | `IsDataValid()` | 编辑器保存时验证 Actions 无空指针 + 禁止蓝图继承蓝图 |
| 第 97~109 行 | `UpdateAssetBundleData()` | 收集所有 Action 的资源需求 |

### 3. `LyraGameMode.h` — GameMode 如何读取 Experience

| 行号 | 内容 | 说明 |
|------|------|------|
| 第 63~64 行 | `GetPawnDataForController()` | 从 Experience 获取 PawnData |
| 第 75 行 | `GetDefaultPawnClassForController_Implementation()` | 动态决定 Pawn 类型 |
| 第 81 行 | `SpawnDefaultPawnAtTransform_Implementation()` | 生成 Pawn 并设置 PawnData |
| 第 136 行 | `OnExperienceLoaded()` | Experience 加载完成后为在场玩家生成 Pawn |

### 4. `LyraGameMode.cpp` — 核心实现

| 行号 | 内容 | 说明 |
|------|------|------|
| 第 60~93 行 | `GetPawnDataForController()` | 查找顺序：PlayerState → Experience → 全局默认 |
| 第 118~190 行 | `HandleMatchAssignmentIfNotExpectingOne()` | 按优先级确定用哪个 Experience（URL→命令行→WorldSettings→Fallback） |
| 第 342~356 行 | `OnExperienceLoaded()` | 为已在场但无 Pawn 的玩家生成角色 |
| 第 373~385 行 | `GetDefaultPawnClassForController_Implementation()` | **核心！** 由 Experience → PawnData → PawnClass 决定，而非硬编码 |
| 第 390~429 行 | `SpawnDefaultPawnAtTransform_Implementation()` | 生成 Pawn + 设置 PawnExtensionComponent |
| 第 440~448 行 | `HandleStartingNewPlayer_Implementation()` | Experience 未加载则等待，避免数据未就绪就生成 |

---

## 一句话总结

> **"同一套代码 + 不同配置 = 不同游戏模式"**

这就是 Experience 系统的核心价值。不需要为每个游戏模式写单独的 C++ 类，只需要创建不同的 `ExperienceDefinition` 数据资产实例即可。
