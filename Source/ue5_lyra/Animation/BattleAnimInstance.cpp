// 战斗角色动画实例cpp - C++和动画蓝图之间的桥梁
#include "BattleAnimInstance.h"
#include "Character/BattleCharacter.h"
#include "Movement/BattleCharacterMovementComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

// 调试计数器（只打印前几帧，避免刷屏）
static int32 DebugFrameCount = 0;

void UBattleAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
    Super::NativeUpdateAnimation(DeltaSeconds);

    // === 调试：确认 NativeUpdateAnimation 被调用 === //
    DebugFrameCount++;
    if (DebugFrameCount <= 5)
    {
        UE_LOG(LogTemp, Warning, TEXT("[AnimInstance] NativeUpdateAnimation 第 %d 帧被调用"), DebugFrameCount);
    }

    // === 第一帧：获取并缓存角色和移动组件指针 === //
    if(!OwnerCharacter)
    {
        // TryGetPawnOwner() 返回拥有这个动画实例的 Pawn
        APawn* Pawn = TryGetPawnOwner();
        if (!Pawn)
        {
            if (DebugFrameCount <= 5)
            {
                UE_LOG(LogTemp, Error, TEXT("[AnimInstance] TryGetPawnOwner() 返回 NULL！"));
            }
            return;
        }
        
        // 尝试获取拥有这个动画实例的角色
        OwnerCharacter = Cast<ABattleCharacter>(Pawn);
        if (!OwnerCharacter)
        {
            if (DebugFrameCount <= 5)
            {
                UE_LOG(LogTemp, Error, TEXT("[AnimInstance] Cast<ABattleCharacter> 失败！Pawn类型 = %s"), *Pawn->GetClass()->GetName());
            }
            return;
        }

        // 获取角色的移动组件
        BattleMovement = Cast<UBattleCharacterMovementComponent>(OwnerCharacter->GetCharacterMovement());
        
        UE_LOG(LogTemp, Warning, TEXT("[AnimInstance] 角色绑定成功！OwnerCharacter = %s, BattleMovement = %s"),
            *OwnerCharacter->GetName(),
            BattleMovement ? TEXT("有效") : TEXT("NULL"));
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

    // 调试：每60帧打印一次当前动画参数
    if (DebugFrameCount % 60 == 0 && DebugFrameCount <= 300)
    {
        UE_LOG(LogTemp, Warning, TEXT("[AnimInstance] Speed=%.1f, bIsInAir=%d, bIsMoving=%d, bIsSprinting=%d"),
            Speed, bIsInAir, bIsMoving, bIsSprinting);
    }
}