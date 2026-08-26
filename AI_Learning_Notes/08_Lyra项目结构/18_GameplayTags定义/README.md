# 十八、GameplayTags 定义

**路径**：`Source/LyraGame/LyraGameplayTags.h/.cpp`

## 核心 Tag 分类

| 分类 | Tag 示例 | 用途 |
|------|----------|------|
| `Ability.ActivateFail.*` | `Ability.ActivateFail.IsDead` | 技能激活失败原因 |
| `Ability.Behavior.*` | `Ability.Behavior.SurvivesDeath` | 技能行为标记 |
| `InputTag.*` | `InputTag.Move`, `InputTag.Look.Mouse` | 输入标签 |
| `InitState.*` | `InitState.Spawned`, `InitState.GameplayReady` | 初始化状态 |
| `GameplayEvent.*` | `GameplayEvent.Death`, `GameplayEvent.Reset` | 游戏事件 |
| `SetByCaller.*` | `SetByCaller.Damage`, `SetByCaller.Heal` | GE 参数传递 |
| `Status.*` | `Status.Crouching`, `Status.Death.Dying` | 状态标记 |
| `Movement.Mode.*` | `Movement.Mode.Walking`, `Movement.Mode.Falling` | 移动模式 |
| `Cheat.*` | `Cheat.GodMode`, `Cheat.UnlimitedHealth` | 作弊标记 |

---

> 📌 详细笔记待补充
