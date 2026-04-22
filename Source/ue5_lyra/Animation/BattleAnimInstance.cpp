// 战斗角色动画实例cpp - C++和动画蓝图之间的桥梁
#include "BattleAnimInstance.h"
#include "Character/BattleCharacter.h"
#include "Movement/BattleCharacterMovementComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

void UBattleAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
    Super::NativeUpdateAnimation(DeltaSeconds);

    // === 第一帧：获取并缓存角色和移动组件指针 === //
    if(!OwnerCharacter)
    {
        // TryGetPawnOwner() 返回拥有这个动画实例的 Pawn
        APawn* Pawn = TryGetPawnOwner();
        if (!Pawn) return;
        
        // 尝试获取拥有这个动画实例的角色
        OwnerCharacter = Cast<ABattleCharacter>(Pawn);
        if (!OwnerCharacter) return;

        // 获取角色的移动组件
        BattleMovement = Cast<UBattleCharacterMovementComponent>(OwnerCharacter->GetCharacterMovement());
    }

    // 如果没有移动组件，直接返回
    if (!BattleMovement) return;

    // === 第二帧及以后：更新动画参数 === //

    // 获取当前速度（只考虑水平速度）
    FVector Velocity = OwnerCharacter->GetVelocity();
    Speed = FVector(Velocity.X, Velocity.Y, 0.0f).Size();

    // 是否在空中
    bIsInAir = BattleMovement->IsFalling();

    // 是否在移动   
    bIsMoving = Speed > 3.0f;

    // 是否在冲刺
    bIsSprinting = BattleMovement->IsSprinting();
}