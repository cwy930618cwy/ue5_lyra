# 一、游戏模式与体验系统（GameModes / Experience）

**路径**：`Source/LyraGame/GameModes/`

## 核心概念
Lyra 采用 **Experience（体验）系统** 实现数据驱动的游戏模式切换，而非传统的硬编码 GameMode。

## 关键类

| 类名 | 职责 |
|------|------|
| `ALyraGameMode` | 游戏模式基类，处理玩家登录、出生、死亡复活 |
| `ALyraGameState` | 游戏状态，持有 `ExperienceManagerComponent` |
| `ULyraExperienceDefinition` | 体验定义（PrimaryDataAsset），配置要启用的 GameFeature、默认 PawnData、Actions 列表 |
| `ULyraExperienceManagerComponent` | 挂在 GameState 上，负责加载/激活/卸载 Experience |
| `ULyraExperienceActionSet` | 可复用的 Action 集合，组合到 Experience 中 |
| `UGameFeatureAction` | GameFeature 动作基类，在 Experience 生命周期各阶段执行 |
| `ALyraWorldSettings` | 关卡设置，可指定该关卡使用的 Experience |
| `ALyraUserFacingExperienceDefinition` | 面向用户的体验定义（带显示名称、图标等 UI 信息） |
| `ALyraBotCreationComponent` | Bot 创建组件，根据配置自动生成 AI 机器人 |

## 工作流程
```
1. WorldSettings 指定 ExperienceID
2. GameState 上的 ExperienceManagerComponent 加载 ExperienceDefinition
3. 启用 GameFeaturesToEnable 中的 GameFeature 插件
4. 执行 Actions 列表（如添加组件、生成 Actor）
5. 使用 DefaultPawnData 生成玩家 Pawn
```

---

> 📌 详细笔记待补充
