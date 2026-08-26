# 一、游戏模式与体验系统（GameModes / Experience）

**路径**：`Source/LyraGame/GameModes/`

## 本目录文件索引

| # | 文件 | 内容 |
|---|------|------|
| 01 | [Experience系统总览](01_Experience系统总览.md) | 整体架构、工作流程时间线、状态机、依赖关系图 |
| 02 | [LyraExperienceDefinition](02_LyraExperienceDefinition_体验定义.md) | 体验定义 DataAsset——4个字段详解、创建方式、设计要点 |
| 03 | [LyraExperienceManagerComponent](03_LyraExperienceManagerComponent_体验管理器.md) | 核心调度器——加载流程源码级拆解、三级回调、网络复制 |
| 04 | [LyraGameMode](04_LyraGameMode_游戏模式.md) | 游戏模式——Experience选择优先级、Pawn动态生成、玩家生命周期 |
| 05 | [LyraGameState](05_LyraGameState_游戏状态.md) | 游戏状态——ExperienceManager宿主、全局ASC、消息广播 |
| 06 | [LyraWorldSettings](06_LyraWorldSettings_关卡设置.md) | 关卡设置——指定默认Experience、软引用设计 |
| 07 | [LyraExperienceActionSet](07_LyraExperienceActionSet_Action集合.md) | Action集合——可复用Action组合包、与Experience的关系 |
| 08 | [LyraUserFacingExperienceDefinition](08_LyraUserFacingExperienceDefinition_面向玩家的体验.md) | 面向玩家的体验——UI展示信息、主菜单集成、与Experience的区别 |
| 09 | [LyraBotCreationComponent](09_LyraBotCreationComponent_Bot创建.md) | Bot创建组件——自动创建AI机器人、名字池机制 |
| 10 | [AsyncAction_ExperienceReady](10_AsyncAction_ExperienceReady_异步等待.md) | 异步等待——蓝图友好的Experience就绪等待 |

## 一句话总结
Lyra 用 **DataAsset 描述玩法配置 + GameStateComponent 调度加载 + GameFeature 插件模块化**，实现了"同一套代码 + 不同配置 = 不同玩法"的数据驱动架构。

## 快速理解：如果你要做一个新游戏模式

1. 创建一个 `LyraExperienceDefinition` 数据资产
2. 填写要启用的 GameFeature、DefaultPawnData、Actions
3. （可选）创建一个 `LyraUserFacingExperienceDefinition` 给它加 UI 信息
4. 在 WorldSettings 里指定这个 Experience
5. 完成！不需要写任何 C++ 代码

---

> 📂 所有文件位于 `Source/LyraGame/GameModes/`
