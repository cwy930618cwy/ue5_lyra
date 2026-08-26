# 二、组队系统（Teams）

**路径**：`Source/LyraGame/Teams/`

## 核心概念
Lyra 的组队系统是 **GameFeature 插件形式** 提供的，支持动态组队、队伍颜色显示、队伍胜负判定。

## 关键类

| 类名 | 职责 |
|------|------|
| `ULyraTeamSubsystem` | 队伍子系统（WorldSubsystem），管理所有队伍信息 |
| `ALyraTeamInfoBase` | 队伍信息基类（Actor），存储队伍公共/私有数据 |
| `FLyraTeamPublicInfo` | 队伍公共信息（所有玩家可见，如队伍名、分数） |
| `FLyraTeamPrivateInfo` | 队伍私有信息（仅队友可见，如战术标记） |
| `ILyraTeamAgentInterface` | 队伍代理接口，PlayerState/Pawn 实现此接口来关联队伍 |
| `ULyraTeamCreationComponent` | 队伍创建组件，处理玩家加入/离开队伍逻辑 |
| `ULyraTeamDisplayAsset` | 队伍显示资源（队标、颜色、名称等 UI 资源） |
| `ULyraTeamStatics` | 队伍静态工具函数（获取队伍 ID、判断是否同队等） |
| `UAsyncAction_ObserveTeam` | 异步动作：监听队伍变化 |
| `UAsyncAction_ObserveTeamColors` | 异步动作：监听队伍颜色变化 |
| `ULyraTeamCheats` | 队伍相关作弊命令 |

## 设计特点
- **公私分离**：PublicInfo 所有人可见，PrivateInfo 仅队友可见
- **接口解耦**：通过 `ILyraTeamAgentInterface` 让任意 Actor 关联队伍
- **事件驱动**：AsyncAction 提供蓝图友好的队伍变化监听

---

> 📌 详细笔记待补充
