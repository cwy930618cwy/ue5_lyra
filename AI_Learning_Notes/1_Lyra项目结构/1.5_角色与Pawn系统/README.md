# 五、角色与 Pawn 系统（Character / Pawn）

**路径**：`Source/LyraGame/Character/`

## 继承体系
```
APawn
 └── ALyraPawn                    ← 最小化 Pawn 基类
      └── ALyraCharacter          ← 带 CharacterMovement 的角色基类
           └── ALyraCharacterWithAbilities  ← 带 GAS 能力的角色
                └── [具体英雄/敌人实现]
```

## 关键类

| 类名 | 职责 |
|------|------|
| `ALyraPawn` | Pawn 基类，实现 `ILyraTeamAgentInterface` |
| `ALyraCharacter` | 角色基类，集成 CMC、相机、输入 |
| `ALyraCharacterWithAbilities` | 带 ASC 的角色，实现 `IAbilitySystemInterface` |
| `ULyraPawnExtensionComponent` | Pawn 扩展组件，协调各组件初始化顺序（InitState 四阶段） |
| `ULyraHeroComponent` | 英雄组件，处理输入绑定、相机切换 |
| `ULyraHealthComponent` | 生命值组件，封装 HealthSet 访问、死亡处理 |
| `ULyraCharacterMovementComponent` | 自定义移动组件 |
| `ULyraPawnData` | Pawn 数据资产，定义 PawnClass + AbilitySets + InputConfig + CameraMode |

## InitState 四阶段初始化
```
Spawned → DataAvailable → DataInitialized → GameplayReady
   ↑            ↑               ↑                ↑
 出生     数据就绪      数据初始化完成      游戏准备完毕
```
由 `PawnExtensionComponent` 协调各组件按序推进状态。

---

> 📌 详细笔记待补充
