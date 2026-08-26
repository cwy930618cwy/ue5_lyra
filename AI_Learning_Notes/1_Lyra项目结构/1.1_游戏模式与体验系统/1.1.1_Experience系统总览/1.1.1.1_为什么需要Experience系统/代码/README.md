# 代码文件夹

## 对应文档
本文件夹中的代码对应 `01_为什么需要Experience系统.md` 文档。

## 文件清单

| 文件 | 对应文档内容 | 说明 |
|------|-------------|------|
| `LyraExperienceDefinition.h` | "把玩法差异从代码中抽出来，放到数据资产里" | 4个字段的定义（GameFeaturesToEnable、DefaultPawnData、Actions、ActionSets） |
| `LyraExperienceDefinition.cpp` | "策划通过创建/修改 DataAsset 就能调整玩法" | 数据验证逻辑（确保策划配置合法） |
| `LyraGameMode.h` | "同一套 C++ 代码 + 不同的 ExperienceDefinition = 不同的游戏模式" | GameMode 如何读取 Experience 配置 |
| `LyraGameMode.cpp` | "不需要写任何新 C++ 代码" | 动态决定 Pawn 类型的实现 |

## 核心代码片段对照

### 文档中的概念 → 代码实现

| 文档描述 | 代码位置 |
|----------|----------|
| `GameFeaturesToEnable: ["ShooterCore", "Teams"]` | `LyraExperienceDefinition.h` 第①个字段 |
| `DefaultPawnData: 标准士兵 vs 快速移动兵` | `LyraExperienceDefinition.h` 第②个字段 |
| `Actions: 队伍UI、队伍计分` | `LyraExperienceDefinition.h` 第③个字段 |
| "不需要写任何新 C++ 代码" | `LyraGameMode.cpp::GetDefaultPawnClassForController()` — 由配置决定 Pawn 类型 |

## 来源
所有源码来自 LyraStarterGame 官方示例项目：
```
e:\ue5\LyraStarterGame\Source\LyraGame\GameModes\
```

---

> ⚠️ 这些是 Epic Games 官方示例代码，版权归 Epic Games 所有。仅用于学习参考。
