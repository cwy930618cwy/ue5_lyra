# 六、玩家系统（Player）

**路径**：`Source/LyraGame/Player/`

## 关键类

| 类名 | 职责 |
|------|------|
| `ALyraPlayerController` | 玩家控制器，处理输入、HUD、相机 |
| `ALyraPlayerState` | 玩家状态，**持有 ASC**（Lyra 把 ASC 放 PlayerState 而非 Character） |
| `ALyraLocalPlayer` | 本地玩家，处理 UI 输入、设置 |
| `ALyraPlayerBotController` | Bot 控制器，AI 控制逻辑 |
| `ALyraPlayerStart` | 玩家出生点，支持队伍/标签过滤 |
| `ULyraPlayerSpawningManagerComponent` | 玩家生成管理组件 |
| `ULyraCheatManager` | 作弊管理器 |
| `ALyraDebugCameraController` | 调试相机控制器 |

## 重要设计
- **ASC 放在 PlayerState**：断线重连时 GE/属性保留，Pawn 销毁重建不影响能力系统

---

> 📌 详细笔记待补充
