# 三、GAS 技能系统（AbilitySystem）

**路径**：`Source/LyraGame/AbilitySystem/`

## 子目录结构
```
AbilitySystem/
├── Abilities/           ← 具体技能实现（GA_*.h/cpp）
├── Attributes/          ← 属性集（AttributeSet）
├── Executions/          ← GE 执行计算（伤害公式等）
├── Phases/              ← 技能阶段（多段技能）
└── [根目录文件]         ← ASC、AbilitySet、Tag 映射等
```

## 关键类

| 类名 | 职责 |
|------|------|
| `ULyraAbilitySystemComponent` | Lyra 的 ASC，扩展了输入绑定、技能激活策略 |
| `ULyraGameplayAbility` | GA 基类，定义 InstancingPolicy/NetExecutionPolicy/ActivationGroup |
| `ULyraAbilitySet` | 技能集 DataAsset，批量授予 GA + GE + AttributeSet |
| `ULyraAbilityTagRelationshipMapping` | Tag 关系映射（如哪些状态会阻止/取消技能） |
| `ULyraGlobalAbilitySystem` | 全局能力系统（用于非 Pawn 实体的 GE 应用） |
| `ALyraAbilitySourceInterface` | 能力来源接口（追踪伤害来源） |
| `FLyraGameplayEffectContext` | 自定义 GE 上下文（携带额外数据） |
| `ULyraGameplayCueManager` | GameplayCue 管理器 |
| `ULyraGameplayAbilityTargetData_SingleTargetHit` | 单目标命中目标数据 |
| `ALyraTaggedActor` | 带 GameplayTag 的 Actor 基类 |

## Abilities 子目录
- `LyraGameplayAbility_Jump.cpp` — 跳跃
- `LyraGameplayAbility_Reset.cpp` — 重置
- `LyraGameplayAbility_Crouch.cpp` — 蹲伏
- `LyraGameplayAbility_AutoRun.cpp` — 自动跑
- `LyraGameplayAbility_PluginBound.cpp` — 插件绑定技能
- `PainfulDeaths/` — 痛苦死亡相关技能
- `Tasks/` — 自定义 AbilityTask

## Attributes 子目录
- `LyraHealthSet.h` — 生命值属性集
- `LyraCombatSet.h` — 战斗属性集

## Executions 子目录
- `LyraDamageExecution.h` — 伤害计算执行
- `LyraHealExecution.h` — 治疗计算执行

## Phases 子目录
- `LyraAbilityPhase.h` — 技能阶段定义
- `LyraAbilitySimplePhases.h` — 简单阶段

---

> 📌 详细笔记待补充
