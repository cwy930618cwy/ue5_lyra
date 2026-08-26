// 游戏角色
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Enums/ABDCombatEnums.h"
#include "BattleCharacter.generated.h"

// 机械臂
class USpringArmComponent;
// 摄像机
class UCameraComponent;
// 输入映射
class UInputMappingContext;
// 输入动作
class UInputAction;
// 自定义输入组件
class UBattleCharacterMovementComponent;
// 动画蒙太奇
class UAnimMontage;
// GAS 能力系统组件
class UAbilitySystemComponent;
// 血量属性集
class UHealthSet;
// 游戏性效果
class UGameplayEffect;
// 血量组件
class UHealthComponent;
// 攻击技能
class UGA_Attack;


// 输入动作值
struct FInputActionValue;

// GAS 事件数据
struct FGameplayEventData;

UCLASS()
class UE5_LYRA_API ABattleCharacter : public ACharacter
{
    GENERATED_BODY()
    
public:
    ABattleCharacter(const FObjectInitializer& ObjectInitializer);  // 构造函数

    // 获取 ASC（其他类（比如敌人）要攻击本角色时需要拿到它）
    UAbilitySystemComponent* GetAbilitySystemComponent() const;

    // 获取 血量
    UHealthSet* GetHealthSet() const { return HealthSet; }

     // 🆕 扣血（Amount 为正数，表示扣多少血）
    UFUNCTION(BlueprintCallable, Category = "Health")
    void ApplyDamage(float Amount);

    // 🆕 加血（Amount 为正数，表示加多少血）
    UFUNCTION(BlueprintCallable, Category = "Health")
    void Heal(float Amount); 

     // 切换战斗状态
    void SetCombatState(ECombatState NewState);

protected:
    virtual void BeginPlay() override;
    // 设置玩家输入组件
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    // 机械臂
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<USpringArmComponent> SpringArm;

    // 摄像机
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UCameraComponent> FollowCamera;

    // 输入映射
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Input")
    TObjectPtr<UInputMappingContext> DefaultMappingContext;

    // 输入动作
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Input")
    TObjectPtr<UInputAction> MoveAction;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Input")
    TObjectPtr<UInputAction> LookAction; 

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Input")
    TObjectPtr<UInputAction> JumpAction;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Input")
    TObjectPtr<UInputAction> SprintAction;

    // 攻击输入动作
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Input")
    TObjectPtr<UInputAction> AttackAction;

    // 攻击动画蒙太奇
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
    TObjectPtr<UAnimMontage> AttackMontage;

    // 受击动画蒙太奇
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
    TObjectPtr<UAnimMontage> HitReactMontage;

    // 自定义移动组件指针（方便直接调用冲刺方法）
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UBattleCharacterMovementComponent> BattleMovement; 

    // GAS 能力系统组件（管技能 + 属性）
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
    TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

    // 血量属性集
    UPROPERTY()
    TObjectPtr<UHealthSet> HealthSet;

    // 测试扣血输入动作（H键）
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Input")
    TObjectPtr<UInputAction> TestTakeDamageAction;

    // 测试加血输入动作（J键）
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Input")
    TObjectPtr<UInputAction> TestHealAction; 

    // 测试显示属性输入动作（K键）
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Input")
    TObjectPtr<UInputAction> TestShowStatsAction;

    // 伤害 GE 模板类 （指向 GE_Damage 蓝图类，不是实例）
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GAS|Effects")
    TSubclassOf<UGameplayEffect> DamageEffectClass;

    // 治疗 GE 模板类 （指向 GE_Heal 蓝图类，不是实例）
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GAS|Effects")
    TSubclassOf<UGameplayEffect> HealEffectClass;

    // 攻击技能类（用于授予 ASC）
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GAS|Abilities")
    TSubclassOf<UGA_Attack> AttackAbilityClass;

    // 血量组件
    UPROPERTY(VisibleAnywhere, Category = "Components")
    TObjectPtr<UHealthComponent> HealthComponent;

    // 当前战斗状态
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
    ECombatState CurrentCombatState = ECombatState::Idle;

    // 输入回调
    void Move(const FInputActionValue& Value);
    void Look(const FInputActionValue& Value);
    void StartJump();
    void StopJump();
    void StartSprint();
    void StopSprint();

    // 攻击
    void Attack();

    // 测试扣血
    void TestTakeDamage();
    void TestHeal();

    // 显示所有属性（调试用）
    void ShowStats();
};
