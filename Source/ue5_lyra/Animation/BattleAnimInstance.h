// 战斗角色动画实例 - C++和动画蓝图之间的桥梁
#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "BattleAnimInstance.generated.h"

// 前向声明（只告诉编译器"有这个类"，不需要 include 完整头文件）
class ABattleCharacter;
class UBattleCharacterMovementComponent;

UCLASS()
class UE5_LYRA_API UBattleAnimInstance : public UAnimInstance
{
    GENERATED_BODY()

public:
    // 每帧更新动画参数（相当于动画蓝图的 Event Blueprint Update Animation）
    virtual void NativeUpdateAnimation(float DeltaSeconds) override;

protected:
    // ===== 动画状态变量（动画蓝图里直接读取这些值） =====

    // 当前移动速度（用于 Idle/Walk/Run 切换）
    UPROPERTY(BlueprintReadOnly, Category = "Animation")
    float Speed = 0.0f;

    // 是否在空中（用于跳跃动画）
    UPROPERTY(BlueprintReadOnly, Category = "Animation")
    bool bIsInAir = false;

    // 是否在移动（速度 > 3 就算在移动）
    UPROPERTY(BlueprintReadOnly, Category = "Animation")
    bool bIsMoving = false;

    // 是否在冲刺（用于 Walk/Run 动画切换）
    UPROPERTY(BlueprintReadOnly, Category = "Animation")
    bool bIsSprinting = false;

private:
    // 缓存的角色指针（避免每帧 Cast）
    UPROPERTY()
    TObjectPtr<ABattleCharacter> OwnerCharacter;

    // 缓存的移动组件指针
    UPROPERTY()
    TObjectPtr<UBattleCharacterMovementComponent> BattleMovement;
};