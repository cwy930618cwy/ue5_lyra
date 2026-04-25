// 游戏角色
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
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

// 输入动作值
struct FInputActionValue;

UCLASS()
class UE5_LYRA_API ABattleCharacter : public ACharacter
{
    GENERATED_BODY()
    
public:
    ABattleCharacter(const FObjectInitializer& ObjectInitializer);  // 构造函数
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

    // 是否正在攻击（防止连续触发）
    UPROPERTY(BlueprintReadOnly, Category = "Combat")
    bool bIsAttacking = false;

    // 自定义移动组件指针（方便直接调用冲刺方法）
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UBattleCharacterMovementComponent> BattleMovement; 

    // 输入回调
    void Move(const FInputActionValue& Value);
    void Look(const FInputActionValue& Value);
    void StartJump();
    void StopJump();
    void StartSprint();
    void StopSprint();
    void Attack();

    // 蒙太奇播放结束回调
    UFUNCTION()
    void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);
};
