# 二十、命名规范总结

## Lyra 命名规范速查表

| 前缀/后缀 | 含义 | 示例 |
|-----------|------|------|
| `Lyra` | 项目前缀 | `LyraCharacter`, `LyraGameMode` |
| `GA_` | GameplayAbility | `GA_Attack`, `LyraGameplayAbility_Jump` |
| `GE_` | GameplayEffect | `GE_Damage`, `GE_Heal` |
| `GC_` | GameplayCue | `GC_HitImpact` |
| `U` | UObject 派生类 | `ULyraAbilitySet` |
| `A` | AActor 派生类 | `ALyraCharacter` |
| `F` | 结构体/非 UObject 类 | `FLyraTeamPublicInfo` |
| `E` | 枚举 | `ECombatState` |
| `I` | 接口 | `ILyraTeamAgentInterface` |
| `b` | 布尔变量 | `bIsDead`, `bCanJump` |
| `K2_` | 蓝图可覆写函数 | `K2_OnDeath` |
| `OnRep_` | 网络回调 | `OnRep_Health` |
| `FindXxx` | 静态查找函数 | `FindHealthComponent()` |

---

> 📌 详细笔记待补充
