# 【AI 游戏开发】从策划到实现 — 虚幻引擎 UE5.7 + Meshy AI + GAS 5.7 + MetaHuman

> **目标**：一个人用 AI 工具从零做出一个可玩的动作战斗 Demo  
> **技术栈**：UE5.7 | C++（不用蓝图）| Meshy AI | GAS | MetaHuman | ActorCore  
> **前置要求**：有编程基础即可，不需要美术能力，不需要建模经验  
> **预计周期**：7 天（每天 3-4 小时）| 总计约 25 小时

---

## 📋 教程总览

### 一天一览

| 天数 | 主题 | 产出 |
|------|------|------|
| **Day 1** | 环境搭建 + 工具链熟悉 | 能跑起来的空白项目 |
| **Day 2** | 策划文档 + Meshy AI 生成资产 | 完整的 GDD + 第一批 3D 模型 |
| **Day 3** | MetaHuman 角色 + ActorCore 动画库 | 可控制的玩家角色 |
| **Day 4** | GAS 技能系统基础框架 | Ability / Attribute / Effect 三件套跑通 |
| **Day 5** | 战斗系统核心实现 | 攻击、受击、伤害、死亡完整流程 |
| **Day 6** | AI 敌人 + 战斗场景搭建 | 可打怪的完整 Demo |
| **Day 7** | 打磨 + 打包发布 | 最终可运行的 exe |

---

## Day 1：环境搭建 + 工具链熟悉（3h）

### 1.1 安装与配置（45min）

```
□ UE5.7 安装（Epic Launcher → Engine Versions → 5.7）
   勾选组件：
   - ✅ Gameplay Ability System 插件
   - ✅ MetaHuman 插件
   - ✅ Online Framework（如需联机）
   
□ VS2022 17.14 已装好 ✓（确认）
   需要勾选：
   - "使用 C++ 的游戏开发"
   - Windows 11 SDK
   
□ Rider 2025.1.6 已装好 ✓（确认）
   
□ Meshy AI 账号注册
   - 免费：每月 200 credits
   - 付费：Pro $20/月（推荐起步阶段用免费额度）
   - 网址：meshy.ai
```

### 1.2 创建 UE5.7 C++ 项目（30min）

```
项目名称：AIBattleDemo
位置：E:\ue5\ue5_battle\AIBattleDemo
模板：Third Person（C++）
目标平台：Windows
无 Starter Content（保持干净）
```

**关键设置**：
```cpp
// AIBattleDemo.Build.cs 中启用插件依赖
PublicDependencyModuleNames.AddRange(new string[] {
    "Core",
    "CoreUObject", 
    "Engine",
    "InputCore",
    "GameplayAbilities",     // GAS
    "GameplayTags",          // GAS 标签
    "GameplayTasks",         // GAS 任务
    "MetaHuman",             // MetaHuman
    "MetaHumanRuntime"       // MetaHuman Runtime
});
```

### 1.3 工具链验证（45min）

| 任务 | 验证方式 | 预期结果 |
|------|---------|---------|
| 编译运行 | F5 或 Ctrl+F5 | 第三人称模板正常运行 |
| Rider 打开 | File → Open → 选择 .uproject | 项目正常加载，C++ 代码可导航 |
| Meshy 连通性 | 浏览器打开 meshy.ai → 输入 "a sword" → Generate | 60 秒内得到 3D 模型预览 |
| MetaHuman 访问 | Epic Launcher → MetaHuman Creator | 可以创建/编辑虚拟人 |
| ActorCore 访问 | Unreal Marketplace → 搜索 "ActorCore" | 可以免费下载动画包 |

### 1.4 目录结构约定（30min）

```
AIBattleDemo/
├── Source/AIBattleDemo/
│   ├── Public/
│   │   ├── AIBattleDemo.h
│   │   ├── AIBattleDemoCharacter.h        # 玩家角色基类
│   │   ├── AIBattleDemoEnemy.h            # 敌人基类
│   │   ├── Abilities/                     # GAS 技能目录
│   │   │   ├── ABDAbilityBase.h           # 所有技能的父类
│   │   │   ├── ABDAttackAbility.h         # 攻击技能
│   │   │   ├── ABDDodgeAbility.h          # 闪避技能
│   │   │   └── ABDUltimateAbility.h       # 大招技能
│   │   ├── Attributes/                    # 属性集
│   │   │   └── ABDAttributeSet.h          # HP/MP/Stamina 等
│   │   ├── Components/                    # 自定义组件
│   │   │   └── ABDCombatComponent.h       # 战斗逻辑组件
│   │   ├── Data/                          # 数据资产
│   │   │   ├── ABDCharacterData.h         # 角色数据表
│   │   │   └── ABDWeaponData.h            # 武器数据表
│   │   ├── Enums/                         # 枚举定义
│   │   │   └── ABDCombatEnums.h           # 战斗相关枚举
│   │   ├── Interfaces/                    # 接口
│   │   │   └── ABDCombatInterface.h       # 战斗交互接口
│   │   └── Subsystems/                    # 子系统
│   │       └── ABDCombatSubsystem.h       # 全局战斗管理
│   └── Private/
│       └── （对应所有 .cpp 文件）
├── Content/
│   ├── _Game/                              # 自定义内容根目录
│   │   ├── Blueprints/                    # 必须用的蓝图数据（尽量少）
│   │   ├── Characters/                    # 角色资源
│   │   ├── Weapons/                       # 武器模型+数据
│   │   ├── Enemies/                       # 敌人预设
│   │   ├── Animations/                    # 动画资源（ActorCore）
│   │   ├── Effects/                       # 特效/VFX
│   │   ├── Maps/                          # 关卡地图
│   │   ├── Audio/                         # 音效
│   │   └── UI/                            # HUD/UI
│   ├── MeshyAI/                           # Meshy 生成的模型存放
│   ├── MetaHuman/                         # MetaHuman 角色导出
│   └── ActorCore/                         # ActorCore 动画导入
├── Plugins/                               # 自定义插件（如有）
└── DesignDocs/                            # 策划文档
```

**Day 1 结束标准**：
- [ ] UE5.7 C++ 项目能编译运行
- [ ] Rider 能正常打开和编辑代码
- [ ] Meshy AI 能生成测试模型
- [ ] 目录结构已建好

---

## Day 2：策划文档 + Meshy AI 生成资产（4h）

### 2.1 游戏策划文档 GDD（1h）

#### 2.1.1 一句话概述
> 一个暗黑风格的第三人称动作战斗 Demo，玩家操控一名剑士，在废墟中与 AI 敌人战斗，展示完整的技能连招和受击反馈系统。

#### 2.1.2 核心玩法

| 维度 | 设计 |
|------|------|
| **视角** | 第三人称过肩视角（TPS），带相机抖动 |
| **操作** | WASD 移动、Space 翻滚闪避、鼠标左键攻击、右键重击、Q 格挡、R 大招 |
| **战斗节奏** | 快节奏，单次战斗 10-30 秒，强调连招和时机判断 |
| **敌人类型** | 3 种：近战兵（快速）、远程弓手（需要躲避）、精英怪（多阶段） |
| **胜利条件** | 清空区域内所有敌人 |
| **失败条件** | 玩家 HP 归零 |

#### 2.1.3 技能设计（基于 GAS）

```
┌─────────────────────────────────────────────────────┐
│                   玩家技能树                          │
├─────────────┬──────────┬───────┬────────┬───────────┤
│ 技能名称     │ 类型      │ 冷却  │ 消耗   │ 效果描述   │
├─────────────┼──────────┼───────┼────────┼───────────┤
│ 普通攻击     │ Active    │ 无    │ 无     │ 三段连击   │
│ 重击         │ Active    │ 2s    │ Stamina│ 击退敌人   │
│ 闪避翻滚     │ Active    │ 1s    │ Stamina│ 无敌帧     │
│ 格挡         │ Active    │ 无    │ Stamina│ 减伤+反    │
│ 剑气斩       │ Active    │ 8s    │ MP     │ 远程AOE   │
│ 终极技·疾风剑│ Ultimate  │ 30s   │ 全部MP  │ 多段斩击   │
│ 被动：战意    │ Passive   │ 无    │ 无     │ 连击增伤   │
└─────────────┴──────────┴───────┴────────┴───────────┘
```

#### 2.1.4 属性系统（Attribute Set）

```cpp
// 核心属性定义
FGameplayAttributeDataDef(Health, 100, 0, 100);       // 生命值
FGameplayAttributeDataDef(MaxHealth, 100, 100, 999);    // 最大生命值
FGameplayAttributeDataDef(Mana, 50, 0, 50);             // 法力值
FGameplayAttributeDataDef(MaxMana, 50, 50, 200);        // 最大法力值
FGameplayAttributeDataDef(Stamina, 100, 0, 100);        // 耐力值
FGameplayAttributeDataDef(MaxStamina, 100, 100, 200);   // 最大耐力
FGameplayAttributeDataDef(AttackPower, 15, 0, 99);      // 攻击力
FGameplayAttributeDataDef(Defense, 5, 0, 50);           // 防御力
FGameplayAttributeDataDef(ComboCount, 0, 0, 10);        // 当前连击数
FGameplayAttributeDataDef(MoveSpeed, 600, 200, 1200);   // 移动速度
```

#### 2.1.5 需要的资产清单

```
【角色类】
□ 玩家角色：黑暗骑士（MetaHuman 定制）
□ 敌人-近战兵：堕落士兵（Meshy AI 生成）
□ 敌人-远程弓手：暗影弓箭手（Meshy AI 生成）
□ 敌人-精英：熔岩巨人（Meshy AI 生成）

【武器类】
□ 长剑 - 玩家主武器（Meshy AI）
□ 弓箭 - 敌人武器（Meshy AI）
□ 巨锤 - 精英怪武器（Meshy AI）

【场景类】
□ 废墟地面/墙壁 - 简单几何体 + 贴图
□ 破碎石柱 - Meshy AI 或 UE 自带
□ 火把/光源 - UE 内置

【特效类】
□ 攻击拖尾（Niagara）
□ 受击闪光（Niagara）
□ 死亡消散（Niagara）
□ 剑气斩 AOE 光效（Niagara）

【UI类】
□ 血条 HUD（UMG）
□ 技能冷却图标（UMG）
□ 伤害数字飘字（UMG）
```

### 2.2 Meshy AI 资产生成实操（3h）

#### 2.2.1 Meshy 提示词技巧

**好的提示词公式**：
```
[主体] + [风格] + [材质] + [姿势/格式] + [质量要求]
```

**实际使用的提示词**：

```
# 玩家长剑（主武器）
"Fantasy dark iron long sword, intricate engravings on blade, 
leather-wrapped handle, game-ready low-poly, clean topology, 
metallic material, dark fantasy style"

# 堕落士兵（普通敌人）
"Dark fantasy undead soldier, tattered armor, rusted helmet, 
holding a broken shield and short sword, hunched posture, 
game-ready character model, low-poly around 8000 tris, 
PBR textures, dark souls aesthetic, T-pose"

# 暗影弓箭手（远程敌人）
"Hooded shadow archer, dark cloak covering face, 
recurve bow with glowing blue arrow quiver, agile stance, 
game-ready character model, assassin style, T-pose, 
low-poly, PBR materials"

# 熔岩巨人（精英怪）
"Lava rock giant, molten cracks glowing orange-red through stone body, 
massive build, club weapon made of volcanic rock, 
intimidating face carved in stone, game-ready boss model, 
around 15000 tris, subsurface scattering for lava effect, T-pose"
```

#### 2.2.2 Meshy 工作流程

```
步骤1: 输入提示词 → 点击 Generate
步骤2: 等待 30-60 秒 → 查看 4 个候选结果
步骤3: 选择最佳结果 → 点击 Refine（精细化）
步骤4: 等待 10 分钟 → 得到高精度模型
步骤5: 导出为 FBX 格式（含贴图）
步骤6: 导入到 UE5.7 Content/MeshyAI/

注意事项：
- 免费版每次生成约消耗 5-10 credits
- Refine 消耗较多 credits，非关键模型可以跳过
- 导出选 FBX + Separate Texture Files（方便 UE 材质调整）
- 如果模型有穿模，在 Blender 里简单修复再导出（可选）
```

#### 2.2.3 批量生成清单

按优先级排序（先做最关键的）：

| 优先级 | 资产 | 提示词关键词 | 预估 Credits |
|--------|------|-------------|-------------|
| 🔴 P0 | 玩家长剑 | Fantasy dark iron long sword | ~10 |
| 🔴 P0 | 堕落士兵 | Undead soldier tattered armor | ~15 |
| 🟡 P1 | 暗影弓箭手 | Hooded shadow archer bow | ~15 |
| 🟡 P1 | 熔岩巨人 | Lava rock giant boss | ~20 |
| 🟢 P2 | 破碎石柱 | Ruined stone pillar debris | ~5 |
| 🟢 P2 | 火把模型 | Medieval torch flame | ~5 |
| ⚪ P3 | 盾牌道具 | Broken soldier shield | ~5 |

**Day 2 结束标准**：
- [ ] GDD 文档完成并保存到 DesignDocs/
- [ ] 至少 P0 的 2 个模型已生成并导入 UE5
- [ ] 模型在 UE5 中能正常显示

---

## Day 3：MetaHuman 角色 + ActorCore 动画库（4h）

### 3.1 创建 MetaHuman 角色（1.5h）

#### 3.1.1 MetaHuman Creator 流程

```
1. 打开 Epic Launcher → MetaHuman Creator
2. 选择基础脸型（推荐选偏硬朗的男性/女性脸型）
3. 调整参数：
   - 脸型：下颌线条硬朗、眉骨突出（战士感）
   - 发型：短黑发/束发（战斗实用）
   - 眼睛：锐利眼神、深色瞳孔
   - 皮肤：略带风霜感（增加故事感）
4. 导出选择：
   - 格式：UE5 Skeleton（兼容 UE5.7 骨骼）
   - 细节级别：High（用于主角）
   - 包含：身体 + 衣服 + 发型
5. 导出到 MetaHuman Plugin → 自动同步到 UE5
```

#### 3.1.2 在 UE5 中配置 MetaHuman

```cpp
// AIBattleDemoCharacter.h
UCLASS()
class AAIBattleDemoCharacter : public ACharacter,
    public IAbilitySystemInterface,
    public IGameplayTagAssetInterface
{
    GENERATED_BODY()

public:
    AAIBattleDemoCharacter();

    // GAS 接口实现
    virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
    virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override;

protected:
    // GAS 核心 Component
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TObjectPtr<UAbilitySystemComponent> AbilitySystemComp;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TObjectPtr<UABDAttributeSet> AttributeSet;

    // 初始化技能
    virtual void PossessedBy(AController* NewController) override;
    virtual void OnRep_Controller() override;
    
    // 给予初始技能
    virtual void InitAbilities();
    
    // 给予初始效果
    virtual void ApplyStartupEffects();

private:
    // 技能列表（TSubclassOf 是软引用）
    UPROPERTY(EditDefaultsOnly, Category = "Abilities")
    TArray<TSubclassOf<UGameplayAbility>> DefaultAbilities;

    // 初始效果列表
    UPROPERTY(EditDefaultsOnly, Category = "Abilities")
    TArray<TSubclassOf<UGameplayEffect>> DefaultEffects;
};
```

### 3.2 ActorCore 动画库集成（2.5h）

#### 3.2.1 什么是 ActorCore

> **ActorCore** 是 Epic 官方提供的免费高质量动画资源库，
> 包含数百套专业制作的动画（走、跑、跳、攻击、受击、死亡等），
> 直接兼容 MetaHuman 骨骼。

**下载地址**：Unreal Marketplace → 搜索 "ActorCore" → 免费

#### 3.2.2 需要下载的动画包

| 动画类别 | ActorCore 中的名称 | 用途 |
|---------|-------------------|------|
| **Locomotion** | AC_Walk_* / AC_Run_* / AC_Idle_* | 移动+待机 |
| **Combat - Attack** | AC_Attack_Melee_1H_* | 单手剑三段连击 |
| **Combat - Attack** | AC_Attack_Heavy_1H_* | 重击动画 |
| **Combat - Hit** | AC_HitReaction_Front/Back/Left/Right | 受击反馈 |
| **Combat - Block** | AC_Block_Start/Loop/End | 格挡动画 |
| **Combat - Death** | AC_Death_* | 死亡动画 |
| **Movement** | AC_Dash/Dodge_Roll_* | 翻滚闪避 |
| **Jump** | AC_Jump_*/AC_Fall_/AC_Land_* | 跳跃落地 |

#### 3.2.3 动画蓝图结构

```
ABP_PlayerAnim（继承自 Mannequin/MetaHuman 骨骼）
│
├── State Machine: CombatSM
│   ├── Idle/Run State（Locomotion）
│   │   └── Blend Space: BS_Locomotion
│   │       ├── Direction X: 左(-1) → 右(1)
│   │       └── Direction Y: 向后(-1) → 向前(1)
│   │
│   ├── Attack State
│   │   ├── Montage: M_Attack_Light_01（左键第一击）
│   │   ├── Montage: M_Attack_Light_02（左键第二击）
│   │   ├── Montage: M_Attack_Light_03（左键第三击）
│   │   └── Montage: M_Attack_Heavy（右键重击）
│   │
│   ├── Dodge State
│   │   └── Montage: M_Dodge_Roll（方向键+空格触发）
│   │
│   ├── Hit Reaction State
│   │   ├── Montage: M_Hit_Front（正面受击）
│   │   ├── Montage: M_Hit_Back（背面受击）
│   │   └── Notify: HitImpact（播放受击音效/特效）
│   │
│   ├── Block State
│   │   └── Montage: M_Block（按住 Q 时进入格挡状态）
│   │
│   └── Death State
│       └── Montage: M_Death（HP=0 时播放）
│
├── Anim Notify Events
│   ├── OnAttackStartNotify → 触发攻击判定盒开启
│   ├── OnAttackHitNotify   → 触发伤害计算（调用 GAS ApplyEffect）
│   ├── OnAttackEndNotify   → 攻击判定盒关闭
│   ├── OnFootstepNotify    → 播放脚步声
│   └── OnLandNotify        → 播放落地尘土特效
│
└── IK（逆向动力学）
    ├── Foot IK → 脚部贴合地形
    └── Hand IK → 手部贴合武器握持点
```

#### 3.2.4 C++ 与动画蓝图通信

```cpp
// 通过 ANIM_NOTIFY 宏自定义 AnimNotify
// 在 C++ 中定义通知事件，蓝图中直接使用

// ABDAttackNotify.cpp
void UABDAttackNotify::Notify(USkeletalMeshComponent* MeshComp, 
                                UAnimSequenceBase* Animation,
                                const FAnimNotifyEventReference& EventReference)
{
    Super::Notify(MeshComp, Animation, EventReference);
    
    if (AAIBattleDemoCharacter* OwnerChar = Cast<AAIBattleDemoCharacter>(MeshComp->GetOwner()))
    {
        if (NotifyType == EABDAttackNotifyType::Hit)
        {
            OwnerChar->OnAttackHit();  // 通知角色：攻击命中时刻到了
        }
        else if (NotifyType == EABDAttackNotifyType::Start)
        {
            OwnerChar->OnAttackStart();  // 攻击开始
        }
        else if (NotifyType == EABDAttackNotifyType::End)
        {
            OwnerChar->OnAttackEnd();    // 攻击结束
        }
    }
}
```

**Day 3 结束标准**：
- [ ] MetaHuman 角色已创建并导入项目
- [ ] ActorCore 动画包已下载并整理到 Content/ActorCore/
- [ ] 基础移动（走/跑/跳/落地）动画可用
- [ ] 至少一套攻击动画已接入

---

## Day 4：GAS 技能系统基础框架（4h）

### 4.1 GAS 核心概念速查（参考手册）

```
┌─────────────────────────────────────────────────────────┐
│                    GAS 架构图                             │
│                                                         │
│  Player Character                                       │
│    ├── AbilitySystemComponent (ASC) ← 管理所有技能/属性  │
│    │     ├── AttributeSet (AS)     ← 数据容器 (HP/MP等) │
│    │     ├── GameplayAbility (GA)  ← 技能定义（"做什么"）│
│    │     ├── GameplayEffect (GE)   ← 效果修改（"改什么"）│
│    │     └── GameplayTag (GT)      ← 标签系统（"谁影响谁"）│
│    │                                                    │
│    └── 流程示例：                                        │
│          按下攻击键                                      │
│            → ASC.TryActivateAbility(GA_Attack)          │
│              → GA.CommitAbility()                       │
│                → GE_Damage 应用给目标                     │
│                  → 目标 ASC.ModifyAttribute(HP -= 15)    │
│                                                         │
└─────────────────────────────────────────────────────────┘
```

### 4.2 实现 Attribute Set（1h）

```cpp
// ABDAttributeSet.h
#pragma once

#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "ABDAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
    GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
    GAMEPLAYATTRIBUTE_PROPERTY_SETTER(ClassName, PropertyName) \
    GAMEPLAYATTRIBUTE_PROPERTY_GETTER_REPLICATED(ClassName, PropertyName) \
    GAMEPLAYATTRIBUTE_PROPERTY_SETTER_REPLICATED(ClassName, PropertyName)

UCLASS()
class UABDAttributeSet : public UAttributeSet
{
    GENERATED_BODY()

public:
    UABDAttributeSet();

    // === 生命属性 ===
    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Health, Category = "Combat")
    FGameplayAttributeData Health;
    ATTRIBUTE_ACCESSORS(UABDAttributeSet, Health);

    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxHealth, Category = "Combat")
    FGameplayAttributeData MaxHealth;
    ATTRIBUTE_ACCESSORS(UABDAttributeSet, MaxHealth);

    // === 法力属性 ===
    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Mana, Category = "Combat")
    FGameplayAttributeData Mana;
    ATTRIBUTE_ACCESSORS(UABDAttributeSet, Mana);

    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxMana, Category = "Combat")
    FGameplayAttributeData MaxMana;
    ATTRIBUTE_ACCESSORS(UABDAttributeSet, MaxMana);

    // === 耐力属性 ===
    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Stamina, Category = "Combat")
    FGameplayAttributeData Stamina;
    ATTRIBUTE_ACCESSORS(UABDAttributeSet, Stamina);

    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxStamina, Category = "Combat")
    FGameplayAttributeData MaxStamina;
    ATTRIBUTE_ACCESSORS(UABDAttributeSet, MaxStamina);

    // === 战斗属性 ===
    UPROPERTY(BlueprintReadOnly, Category = "Combat")
    FGameplayAttributeData AttackPower;
    ATTRIBUTE_ACCESSORS(UABDAttributeSet, AttackPower);

    UPROPERTY(BlueprintReadOnly, Category = "Combat")
    FGameplayAttributeData Defense;
    ATTRIBUTE_ACCESSORS(UABDAttributeSet, Defense);

    UPROPERTY(BlueprintReadOnly, Category = "Combat")
    FGameplayAttributeData ComboCount;
    ATTRIBUTE_ACCESSORS(UABDAttributeSet, ComboCount);

protected:
    // 属性变化回调
    virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
    
    // 属性生效后回调（执行 GameEffect 后）
    virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
    
    // 当属性被 clamp 后调用（比如 HP 不能超过 MaxHP）
    virtual void ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const override;

    // Replication 通知函数
    UFUNCTION()
    void OnRep_Health(const FGameplayAttributeData& OldValue);
    UFUNCTION()
    void OnRep_MaxHealth(const FGameplayAttributeData& OldValue);
    UFUNCTION()
    void OnRep_Mana(const FGameplayAttributeData& OldValue);
};
```

```cpp
// ABDAttributeSet.cpp 关键部分
void UABDAttributeSet::PostGameplayEffectExecute(
    const FGameplayEffectModCallbackData& Data)
{
    Super::PostGameplayEffectExecute(Data);

    // HP 归零 → 触发死亡
    if (Data.EvaluatedData.Attribute == GetHealthAttribute())
    {
        if (GetCurrentHealth() <= 0.f && GetCurrentHealth() > -100.f)
        {
            // 广播死亡事件（让角色知道该死了）
            // 注意：这里不直接操作角色，通过委托解耦
        }
        
        // HP 上限不能超过 MaxHP
        if (GetCurrentHealth() > GetCurrentMaxHealth())
        {
            SetHealth(GetCurrentMaxHealth());
        }
    }

    // 耐力自然恢复逻辑也可以放在这里
}
```

### 4.3 实现基础技能类（1.5h）

```cpp
// ABDAbilityBase.h — 所有自定义技能的父类
#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "Abilities/GameplayAbility.h"
#include "ABDAbilityBase.generated.h"

UCLASS(Abstract)
class UABDAbilityBase : public UGameplayAbility
{
    GENERATED_BODY()

public:
    UABDAbilityBase();

    // === 技能配置（子类可在 Details 面板中设置）===
    
    // 冷却时间（秒）
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability")
    float CooldownDuration;

    // 消耗耐力
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability")
    float CostStamina;

    // 消耗法力
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability")
    float CostMana;

    // 技能标签
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability")
    FGameplayTagContainer AbilityTags;

    // 阻塞标签（有这些标签时不能施放）
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability")
    FGameplayTagContainer ActivationBlockedTags;

    // 技能类型标签（用于判断是攻击还是防御等）
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability")
    FGameplayTagContainer AbilityTypeTags;

protected:
    // 技能激活入口
    virtual void ActivateAbility(
        const FGameplayAbilitySpecHandle Handle,
        const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayActivationInfo ActivationInfo,
        const FGameplayEventData* TriggerEventData) override;

    // 技能结束（成功/取消都会调这里）
    virtual void EndAbility(
        const FGameplayAbilitySpecHandle Handle,
        const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayActivationInfo ActivationInfo,
        bool bReplicateEndAbility, bool bWasCancelled) override;

    // 是否可以激活
    virtual bool CanActivateAbility(
        const FGameplayAbilitySpecHandle Handle,
        const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayTagContainer* SourceTags,
        const FGameplayTagContainer* TargetTags,
        FGameplayTagContainer* OptionalRelevantTags) const override;

    // 施放技能时消耗资源（Stamina/Mana）
    virtual void ApplyCost(const FGameplayAbilitySpecHandle Handle,
        const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayActivationInfo ActivationInfo) const override;

    // 施放技能后应用冷却
    virtual void ApplyCooldown(
        const FGameplayAbilitySpecHandle Handle,
        const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayActivationInfo ActivationInfo) const override;

private:
    // 获取拥有者角色的指针（便捷方法）
    AAIBattleDemoCharacter* GetOwningCharacter() const;
};
```

### 4.4 实现具体技能：普通攻击（1.5h）

```cpp
// ABDAttackAbility.h
#pragma once

#include "ABDAbilityBase.h"
#include "ABDAttackAbility.generated.h"

UCLASS()
class UABDAttackAbility : public UABDAbilityBase
{
    GENERATED_BODY()

public:
    UABDAttackAbility();

    // 攻击配置
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack")
    int32 ComboIndex;           // 第几段连击（0/1/2）

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack")
    float BaseDamage;           // 基础伤害值

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack")
    TSubclassOf<UGameplayEffect> DamageEffectClass;  // 伤害 GE 类

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack")
    UAnimMontage* AttackMontage; // 对应攻击动画

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack")
    FName AttackSectionName;    // Montage Section 名称

protected:
    virtual void ActivateAbility(
        const FGameplayAbilitySpecHandle Handle,
        const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayActivationInfo ActivationInfo,
        const FGameplayEventData* TriggerEventData) override;

    // Montage 播放完成的回调
    UFUNCTION()
    void OnAttackMontageCompleted(UAnimMontage* Montage, bool bInterrupted);

    // Montage 播放到特定 Notify 的回调（命中判定）
    UFUNCTION()
    void OnAttackMontageNotify(FName NotifyName, 
        const FBranchingPointNotifyPayload& BranchingPointPayload);
};
```

```cpp
// ABDAttackAbility.cpp 核心逻辑
void UABDAttackAbility::ActivateAbility(...)
{
    // 1. 先调用父类（会检查是否被阻止、是否有足够资源等）
    if (!CommitAbility(Handle, ActorInfo, ActivationInfo, 
        nullptr, nullptr, this))
    {
        EndAbility(...);
        return;
    }

    // 2. 播放攻击动画
    AAIBattleDemoCharacter* Char = GetOwningCharacter();
    if (Char && AttackMontage)
    {
        // 设置连击蒙太奇 section（第1/2/3击用同一个 Montage 的不同 Jump）
        float PlayRate = 1.0f;
        Char->PlayAnimMontage(AttackMontage, PlayRate, AttackSectionName);

        // 绑定完成回调
        FOnMontageCompleted Delegate;
        Delegate.BindUObject(this, &ThisClass::OnAttackMontageCompleted);
        Char->GetAbilitySystemComponent()->AddMontageDelegate(
            Delegate, Handle);
    }

    // 3. 给自身添加 "Attacking" 标签（防止重复攻击等）
    Char->GetAbilitySystemComponent()->AddLooseGameplayTag(
        FGameplayTag::RequestGameplayTag("State.Attacking"));

    // 4. 增加 ComboCount（连击计数）
    UAbilitySystemComponent* ASC = Char->GetAbilitySystemComponent();
    ASC->SetNumericAttributeBase(
        UABDAttributeSet::GetComboCountAttribute(), 
        ComboIndex + 1);
}

void UABDAttackAbility::OnAttackMontageNotify(FName NotifyName, ...)
{
    // AnimNotify 告诉我们："现在是命中帧！"
    if (NotifyName == "AttackHit")
    {
        PerformAttackCheck();  // 见下方
    }
}

void UABDAttackAbility::PerformAttackCheck()
{
    AAIBattleDemoCharacter* Char = GetOwningCharacter();
    if (!Char) return;

    // 方式1：简单球形检测（适合新手入门）
    FVector TraceStart = Char->GetActorLocation() + 
        Char->GetActorForwardVector() * 100.f;  // 身体前方 100cm
    float Radius = 80.f;  // 攻击范围半径
    
    TArray<FHitResult> Hits;
    UKismetSystemLibrary::SphereTraceMultiForObjects(
        Char,
        TraceStart,
        TraceStart,  // 球形检测起点终点相同
        Radius,
        {},  // Object Types（可配置哪些层可以被攻击）
        ETraceTypeQuery_Visible,
        false,  // 不追踪复杂碰撞
        Hits,
        true,  // Debug Draw
        FLinearColor::Red,
        FLinearColor::Green,
        5.f   // Draw Time
    );

    // 对每个命中目标应用伤害
    for (const FHitResult& Hit : Hits)
    {
        if (AActor* TargetActor = Hit.GetActor())
        {
            if (IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(TargetActor))
            {
                UAbilitySystemComponent* TargetASC = ASI->GetAbilitySystemComponent();
                
                // 构造伤害 GE 的 Spec（上下文数据容器）
                FGameplayEffectContextHandle Context = 
                    Char->GetAbilitySystemComponent()->MakeEffectContext();
                Context.AddSourceObject(this);  // 来源是这个技能
                Context.AddHitResult(Hit);

                // 创建 GE Spec 并应用到目标
                FGameplayEffectSpecHandle SpecHandle = 
                    Char->GetAbilitySystemComponent()->MakeOutgoingSpec(
                        DamageEffectClass,  // 伤害 GE 类
                        BaseDamage * (1.f + ComboIndex * 0.2f),  // 连击增伤 20%
                        Context);

                TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data);
            }
        }
    }
}
```

### 4.5 伤害 GameplayEffect 配置（在 C++ 中定义，BP 中微调）

```cpp
// ABDGameplayEffects.h — 定义常用的 GE 子类
UCLASS()
class UABDDamageGameplayEffect : public UGameplayEffect
{
    GENERATED_BODY()
public:
    UABDDamageGameplayEffect();
};

// 在构造函数中设置默认值
UABDDamageGameplayEffect::UABDDamageGameplayEffect()
{
    // Duration Type: Instant（即时生效，不是持续效果）
    DurationPolicy = EGameplayEffectDurationType::Instant;

    // 修改 Health 属性（减去数值 = 伤害）
    Modifiers.Add(FGameplayModifierInfo{
        .Attribute = UABDAttributeSet::GetHealthAttribute(),
        .Magnitude = FScalableFloat(-1.f),  // -1 表示由 Spec 的 Level 决定最终值
        .ModifierOp = EGameplayModOp::Additive,  // 加法（负数就是减法）
        .SourceTagFilter = FGameplayTagContainer{},  // 来源不限
        .TargetTagFilter = FGameplayTagContainer{}   // 目标不限
    });

    // 标签
    InheritedOwnedTagsContainer.AddTag(
        FGameplayTag::RequestGameplayTag("Effect.Damage"));
}
```

**Day 4 结束标准**：
- [ ] AttributeSet 正确注册，HP/Mana/Stamina 能在运行时查看
- [ ] ABDAbilityBase 基类可用
- [ ] 普通攻击技能能正确激活、播放动画、消耗耐力
- [ ] 伤害 GE 能正确扣除目标 HP
- [ ] 冷却系统能正常工作

---

## Day 5：战斗系统核心实现（4h）

### 5.1 战斗状态机（1h）

```cpp
// ABDCombatEnums.h
namespace EABDCombatState
{
    enum Type : uint8
    {
        Idle,           // 待机（可移动、可攻击）
        Moving,         // 移动中
        Attacking,      // 攻击中（不可打断或有限次可打断）
        Dodging,        // 翻滚闪避中（无敌帧）
        Blocking,       // 格挡中
        HitReacting,    // 受击硬直中
        Dead,           // 死亡
        Stunned,        // 眩晕（控制效果）
        Casting,        // 施放大招中
    };
}

// ABDCombatComponent.h — 战斗逻辑核心组件
UCLASS(ClassGroup=(Combat), meta=(BlueprintSpawnableComponent))
class UABDCombatComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UABDCombatComponent();

    // === 状态查询 ===
    bool IsAlive() const { return CurrentState != EABDCombatState::Dead; }
    bool CanAct() const { return CanMove() && CanAttack(); }
    bool CanMove() const { return CurrentState == EABDCombatState::Idle || CurrentState == EABDCombatState::Moving; }
    bool CanAttack() const { return IsAlive() && !IsInActionState(); }
    bool IsInActionState() const;  // Attacking/Dodging/Blocking/Casting 等

    // === 状态切换 ===
    void SetCombatState(EABDCombatState::Type NewState);
    FOnCombatStateChanged& OnStateChanged() { return StateChangedDelegate; }

    // === 事件委托 ===
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDamageReceived, 
        float, ActualDamage, const FHitResult&, HitInfo);
    FOnDamageReceived& OnDamageReceivedDelegate() { return DamageReceivedDelegate; }

    DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeath);
    FOnDeath& OnDeathDelegate() { return DeathDelegate; }

protected:
    virtual void BeginPlay() override;

    UPROPERTY(VisibleAnywhere, Category = "Combat")
    EABDCombatState::Type CurrentState = EABDCombatState::Idle;

    UPROPERTY(VisibleAnywhere, Category = "Combat")
    EABDCombatState::Type PreviousState = EABDCombatState::Idle;

private:
    FOnCombatStateChanged StateChangedDelegate;
    FOnDamageReceived DamageReceivedDelegate;
    FOnDeath DeathDelegate;
};
```

### 5.2 受击反馈系统（1.5h）

```
受击反馈 = 视觉 + 听觉 + 手感的综合表现
```

```cpp
// ABDHitReactionSystem.h
UCLASS()
class UABDHitReactionSystem : public UObject
{
    GENERATED_BODY()

public:
    // 处理一次受击
    void ProcessHit(
        AAIBattleDemoCharacter* Victim,
        float DamageAmount,
        const FHitResult& HitResult,
        const FGameplayTagContainer& SourceTags
    );

protected:
    // 根据攻击方向选择受击动画
    UAnimMontage* ChooseHitReactionMontage(
        AAIBattleDemoCharacter* Victim,
        const FVector& ImpactDirection);

    // 应用受击硬直
    void ApplyStun(
        AAIBattleDemoCharacter* Victim,
        float StunDuration);

    // 播放受击特效
    void SpawnHitVFX(
        const FVector& Location,
        const FVector& Normal);

    // 播放受击音效
    void PlayHitSFX(
        AAIBattleDemoCharacter* Victim,
        EABDPHYSicalMaterialType MatType);

    // 相机震屏
    void ApplyCameraShake(
        AAIBattleDemoCharacter* Victim,
        float Intensity);

    // 时间膨胀（子弹时间效果，用于重击）
    void ApplySlowMotion(
        float Duration,
        float TimeDilation = 0.3f);
};
```

### 5.3 连击系统（1h）

```cpp
// ABDComboManager.h — 连击管理器
UCLASS()
class UABDComboManager : public UObject
{
    GENERATED_BODY()

public:
    struct FComboNode
    {
        int32 Index;                    // 第几段
        TSubclassOf<UGameplayAbility> AbilityClass;  // 这段的技能
        UAnimMontage* Montage;          // 动画
        float InputWindowBegin;         // 输入窗口起始（相对动画开始的时间点，单位秒）
        float InputWindowEnd;           // 输入窗口结束
        float NextComboResetTime;       // 超过这个时间没输入就重置连击
        
        FComboNode() : Index(0), Montage(nullptr),
            InputWindowBegin(0.3f), InputWindowEnd(0.8f), NextComboResetTime(1.5f) {}
    };

    // 尝试输入下一次攻击（返回 true 表示成功接上连击）
    bool TryContinueCombo();

    // 重置连击
    void ResetCombo();

    // 获取当前连击段数
    int32 GetCurrentComboIndex() const { return CurrentComboIndex; }

protected:
    // 连击链配置（在 BP Editor 或 Data Asset 中可视化编辑）
    UPROPERTY(EditDefaultsOnly, Category = "Combo")
    TArray<FComboNode> ComboChain;

    UPROPERTY(VisibleAnywhere, Category = "Combo")
    int32 CurrentComboIndex;

    UPROPERTY(VisibleAnywhere, Category = "Combo")
    float LastAttackTime;

    UPROPERTY(EditDefaultsOnly, Category = "Combo")
    float ComboResetTimeout;  // 默认 2 秒没操作就重置

    FTimerHandle ComboResetTimer;
};
```

**连击时序图**：

```
时间轴 →
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

第1击:  [====攻击动画====]
         ↑开始          ↑InputWindow开始 ↑InputWindow结束
                                              ↓用户按下攻击
第2击:                      [========攻击动画========]
                                             ↑InputWindow开始
                                                                  ↑按下
第3击:                                                        [===攻击===]

超时重置: 如果 InputWindowEnd 之后 ComboResetTimeout 内没有新输入
          → ComboIndex 回到 0，下次攻击从头开始
```

### 5.4 伤害数字飘字（0.5h）

```cpp
// ABDFloatingDamageWidget.h — 伤害数字 Widget
UCLASS()
class UABDFloatingDamageWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    // 显示伤害数字（静态工厂方法）
    static void ShowDamageNumber(
        UWorld* World,
        const FVector& TargetLocation,
        float DamageAmount,
        bool bIsCriticalHit = false,
        bool bIsHealing = false
    );

protected:
    UPROPERTY(meta = (BindWidget))
    UTextBlock* DamageText;

    UPROPERTY(meta = (BindWidgetAnim), Transient)
    UWidgetAnimation* FadeInAndUpAnimation;

    UPROPERTY(EditDefaultsOnly, Category = "Visual")
    FLinearColor NormalDamageColor;     // 白色
    UPROPERTY(EditDefaultsOnly, Category = "Visual")
    FLinearColor CriticalHitColor;     // 金黄色
    UPROPERTY(EditDefaultsOnly, Category = "Visual")
    FLinearColor HealingColor;         // 绿色
};
```

**Day 5 结束标准**：
- [ ] 战斗状态机能正确切换（Idle ↔ Attacking ↔ HitReacting ↔ Dead）
- [ ] 受击时有动画反馈 + 特效 + 相机震动
- [ ] 连击系统工作：三段连击流畅衔接
- [ ] 伤害飘字显示正确
- [ ] 死亡动画播放后角色不可再操作

---

## Day 6：AI 敌人 + 战斗场景搭建（4h）

### 6.1 AI 行为树（BT）（2h）

```
┌─────────────────────────────────────────────────┐
│                 BT_EnemyBehavior                 │
│                                                 │
│  Selector (Root)                                │
│  ├── ① Blackboard: IsDead?                     │
│  │     └── BTTask_Die (播放死亡动画)           │
│  │                                               │
│  ├── ② Blackboard: IsStunned?                  │
│  │     └── BTTask_Stun (眩晕等待)               │
│  │                                               │
│  ├── ③ Selector: Combat Decisions               │
│  │     ├── Blackboard: DistanceToPlayer < 300?  │
│  │     │   └── Sequence: Melee Attack           │
│  │     │       ├── BTTask_ChangeState(Attacking)│
│  │     │       ├── BTService_ChooseAttackType   │
│  │     │       └── BTTask_ActivateAbility       │
│  │     │                                         │
│  │     ├── Blackboard: DistanceToPlayer < 1000? │
│  │     │   └── Sequence: Chase                  │
│  │     │       ├── BTTask_MoveToPlayer          │
│  │     │       └── BTService_UpdateDistance     │
│  │     │                                         │
│  │     └── Blackboard: IsRanged?                │
│  │         └── Sequence: Ranged Attack           │
│  │             ├── BTTask_FindCoverLocation     │
│  │             ├── BTTask_MoveToCover            │
│  │             ├── BTService_AimAtPlayer         │
│  │             └── BTTask_ShootProjectile        │
│  │                                               │
│  └── ④ Sequence: Patrol (默认巡逻)              │
│      ├── BTTask_GetNextPatrolPoint             │
│      ├── BTTask_MoveToPatrolPoint              │
│      └── BTTask_WaitRandom(2~5s)               │
│                                                 │
└─────────────────────────────────────────────────┘
```

#### 6.1.1 AI Controller

```cpp
// AIBattleDemoAIController.h
UCLASS()
class AAIBattleDemoAIController : public AAIController
{
    GENERATED_BODY()

public:
    AAIBattleDemoAIController();

    virtual void OnPossess(APawn* InPawn) override;

protected:
    // 行为树组件
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TObjectPtr<UBehaviorTreeComponent> BehaviorTreeComp;

    // 黑板组件
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TObjectPtr<UBlackboardComponent> BlackboardComp;

    // 黑板 Key 名称
    static const FName KEY_IsDead;
    static const FName KEY_IsStunned;
    static const FName KEY_TargetActor;
    static const FName KEY_DistanceToPlayer;
    static const FName KEY_IsRanged;
    static const FName KEY_CurrentPatrolPoint;
};
```

### 6.2 敌人类派生（1h）

```cpp
// AIBattleDemoEnemy.h
UCLASS()
class AAIBattleDemoEnemy : public AAIBattleDemoCharacter
{
    GENERATED_BODY()

public:
    AAIBattleDemoEnemy();

    // 敌人类型
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
    EABDEnemyType EnemyType;  // Melee/Ranged/Elite

    // 掉落物
    UPROPERTY(EditDefaultsOnly, Category = "Loot")
    TSubclassOf<AActor> LootDropClass;

    // 经验值
    UPROPERTY(EditAnywhere, Category = "Enemy")
    int32 ExpReward;

protected:
    virtual void BeginPlay() override;

    // 死亡时掉落 + 给玩家加经验
    virtual void OnDeath() override;

    // 敌人感知（看到/听到玩家）
    UPROPERTY(VisibleAnywhere)
    TObjectPtr<UPerceptionComponent> PerceptionComp;

    // AI 感知配置
    UPROPERTY(EditDefaultsOnly)
    UAISenseConfig_Sight* SightConfig;
    
    UPROPERTY(EditDefaultsOnly)
    UAISenseConfig_Hearing* HearingConfig;

    UFUNCTION()
    void OnPerceptionUpdated(TArray<AActor*> UpdatedActors);
};
```

### 6.3 战斗场景搭建（1h）

```
MAP_BattleArena（主关卡）

布局俯视图：

                    ┌─────────────────────┐
                    │   [Elite Spawn]      │
                    │     精英怪出生点      │
                    │                      │
    [Archer Spawn]  │                      │  [Archer Spawn]
    ════════════    │                      │  ════════════
    弓箭手掩体      │                      │  弓箭手掩体
                    │                      │
                    │     ★ 玩家出生点      │
                    │    (Player Spawn)     │
                    │                      │
    [Melee Spawn]   │    ▓▓▓▓▓▓▓▓▓▓▓▓▓▓  │  [Melee Spawn]
    ════════════    │    中央竞技场地面     │  ════════════
    近战兵出生      │                      │  近战兵出生
                    └─────────────────────┘

光照设置：
- 主光源：DirectionalLight（暖色调夕阳，增强氛围）
- 点光源：火把（动态光，随距离衰减）
- 后处理：Bloom + 色差（受伤时加强）+ Vignette

音效：
- BGM：暗黑风格管弦乐循环
- 环境音：风声、远处低沉轰鸣
- 战斗音：刀剑碰撞、脚步声、怒吼
```

**Day 6 结束标准**：
- [ ] 近战敌人能发现玩家 → 追踪 → 近身攻击
- [ ] 远程敌人能找掩体 → 射击
- [ ] 精英怪有多阶段行为
- [ ] 敌人死亡后有掉落/经验奖励
- [ ] 场景可玩：玩家能在地图里跑动并与敌人战斗

---

## Day 7：打磨 + 打包发布（3h）

### 7.1 音效与特效完善（1h）

| 内容 | 工具/来源 | 说明 |
|------|----------|------|
| 攻击音效 | freesound.org 或自录 | 刀剑挥舞、命中不同材质 |
| 脚步声 | UE Footstep System | 根据地面材质自动切换 |
| 受击音效 | Niagara + Sound Cue | 金属/皮革/石头不同声音 |
| BGM | Suno AI 或 Epidemic Sound | 暗黑风格背景音乐 |
| 攻击拖尾 | Niagara Ribbon Trail | 拖尾长度与攻击强度关联 |
| 受击闪光 | Niagara Burst | 帧闪白光 + 粒子溅射 |
| 死亡消散 | Niagara Dissolve | 灰飞烟灭效果 |
| 地面打击波纹 | Niagara Decal | 武器砸地的冲击波纹 |

### 7.2 UI 打磨（1h）

#### 7.2.1 HUD 布局

```
┌──────────────────────────────────────────────────┐
│                                                  │
│  ████████████  HP Bar                            │
│  ░░░░░░░░░░░  Stamina Bar                       │
│  ▓▓▓▓▓▓▓▓▓▓  Mana Bar                          │
│                                                  │
│                                                  │
│                                                  │
│              （游戏画面）                          │
│                                                  │
│                                                  │
│  [LMB] 攻击  [RMB] 重击  [SPACE] 翻滚           │
│  [Q] 格挡   [R] 大招    [x3] 连击数              │
│                                                  │
└──────────────────────────────────────────────────┘
```

#### 7.2.2 技能冷却 UI

```cpp
// ABDSkillIconWidget.h — 技能图标 + 冷却遮罩
UCLASS()
class UABDSkillIconWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    void InitializeFromAbility(TSubclassOf<UGameplayAbility> AbilityClass);
    void UpdateCooldown(float RemainingTime, float CooldownTime);

protected:
    UPROPERTY(meta = (BindWidget))
    UImage* SkillIcon;

    UPROPERTY(meta = (BindWidget))
    UOverlay* CooldownOverlay;  // 半透明黑色遮罩，根据 CD 比例缩放

    UPROPERTY(meta = (BindWidget))
    UTextBlock* KeyHintText;   // "LMB"/"RMB"/"Q" 等

    UPROPERTY(meta = (BindWidget))
    UTextBlock* CooldownText;  // "2.3s"
};
```

### 7.3 性能优化 + 打包（1h）

```
优化清单：
□ 降低 Meshy AI 模型的 LOD（Level of Detail）
  - 远距离模型面数降至 500 以下
  - 使用 Nanite 如可能（UE5.7 默认支持）

□ Niagara 特效限制粒子数量
  - 同屏最多 50 个粒子发射器
  - 低端 GPU 自动降低粒子密度

□ 音效池化（避免频繁 Load/Unload）
  - 预加载常用音效到内存
  - 使用 Concurrency 限制同时播放数量

□ AI 性能
  - 敌人感知更新频率降低到 0.5 秒（默认 0.05 秒太频繁）
  - 距离 > 5000cm 的敌人暂停 AI Tick（改用简单的计时器）

□ 打包设置：
  Project Settings → Packaging:
  - Build Configuration: Shipping
  - Use Pak File: ✅
  - Generate Chunks: ✅
  - Include Debug Symbols: ❌（减小体积）

打包命令：
  - 菜单：File → Package Project → Windows (64-bit)
  - 或命令行：
    "C:\Program Files\Epic Games\UE_5.7\Engine\Build\BatchFiles\RunUAT.bat" BuildCookRun ...
```

**Day 7 结束标准（= 教程完成！）**：
- [ ] 所有音效到位
- [ ] HUD 完整且美观
- [ ] 技能冷却 UI 实时更新
- [ ] 打包后的 exe 能独立运行
- [ ] 文件大小控制在 500MB 以内

---

## 附录

### A. 常见问题排查

| 问题 | 可能原因 | 解决方案 |
|------|---------|---------|
| 编译报错找不到 GAS 头文件 | Build.cs 没加 GameplayAbilities 模块 | 检查 PublicDependencyModuleNames |
| GAS 技能无法激活 | AbilitySystemComponent 没初始化 | 确认在 PossessedBy 中 InitAbilityActorInfo |
| MetaHuman 动画变形 | 骨骼不匹配 | 确保 Animator 使用 MetaHuman 骨架而非默认 Mannequin |
| Meshy 模型导入后材质丢失 | FBX 导出时没包含嵌入贴图 | 重新导出选 "Embed Media" 或单独导入贴图 |
| 连击输入吃不掉 | Input Window 太窄或重叠 | 调整 ComboNode.InputWindowBegin/End |
| 敌人不动 | AI Controller 没指定 BehaviorTree | 检查 AIController 的 BrainBlackboardAsset 和 RunBehaviorTree |

### B. 推荐学习顺序

```
如果完全不懂 GAS：
  1️⃣ 先看 Tom Looman 的 GAS 文档（tomlooman.com/unreal-engine-cpp-guide/）
  2️⃣ 再看 CodeLikeMe 的 YouTube GAS 系列（216集）
  3️⃣ 最后跟本教程动手做

如果懂一点 UE C++ 但没用过 GAS：
  1️⃣ 直接从本教程 Day 4 开始
  2️⃣ 遇到问题再看 CodeLikeMe 的对应章节

如果已经是 UE 老手：
  1️⃣ 直接看 Day 4-6 的代码架构
  2️⃣ 重点看 GAS + MetaHuman + ActorCore 的集成方式
```

### C. 扩展方向（做完 7 天之后）

| 方向 | 说明 | 预估工作量 |
|------|------|-----------|
| **更多武器类型** | 双刀、大斧、长枪 | +3 天 |
| **装备系统** | 武器切换 + 装备面板 | +3 天 |
| **存档系统** | SLG（SaveLoadGame）本地存档 | +2 天 |
| **Boss 多阶段** | 精英怪 3 个阶段 + 场景互动 | +3 天 |
| **联机对战** | 在线子系统 + 网络复制 | +7 天 |
| **技能树升级** | RPG 式技能加点 | +3 天 |
| **任务系统** | 接任务 → 完成 → 奖励 | +3 天 |

---

> **版本信息**
> - 教程版本：v1.0 | 2026-04-18
> - UE 版本：5.7 | VS2022: 17.14 | Rider: 2025.1.6
> - 作者：AI 辅助设计 | 基于 WorkBuddy Claw 项目
