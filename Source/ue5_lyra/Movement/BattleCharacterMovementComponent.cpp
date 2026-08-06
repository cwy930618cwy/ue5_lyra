// 角色移动组件 自定义cpp
#include "BattleCharacterMovementComponent.h"

UBattleCharacterMovementComponent::UBattleCharacterMovementComponent()
{
    MaxWalkSpeed = WalkSpeed; // 设置最大的行走速度
    JumpZVelocity = 600.0f; // 设置默认的跳跃高度
    AirControl = 0.2f; // 设置默认的空中的控制能力

    MaxAcceleration = 2000.0f; // 设置默认的最大加速度
    GroundFriction = 8.0f; // 设置默认的地面摩擦力
    BrakingDecelerationWalking = 2048.0f; // 设置默认的行走时的制动减速度
}

// 设置是否想要冲刺
void UBattleCharacterMovementComponent::SetWantsToSprint(bool bSprint)
{
    bWantsToSprint = bSprint;
}

// 是否在冲刺
bool UBattleCharacterMovementComponent::IsSprinting() const
{
    return bIsSprinting;
}

// 开始冲刺
void UBattleCharacterMovementComponent::StartSprint()
{
    SetWantsToSprint(true);
}

// 停止冲刺
void UBattleCharacterMovementComponent::StopSprint()
{
    SetWantsToSprint(false);
}

// 每帧更新，检查冲刺状态
void UBattleCharacterMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    // 是否在地面上
    const bool bOnGround = IsMovingOnGround();

    // 维护 “离开地面后的冲刺保留计时”
    if (bOnGround)
    {
        AirTimeAccumulator  = 0.0f;
    }
    else
    {
        AirTimeAccumulator += DeltaTime;
    }

    // 冲刺有效条件：想冲刺 + 有速度，且（在地面 或 离开地面还在保留时间内）
    const bool bSprintActive = bWantsToSprint && Velocity.Size2D() > 10.0f
        && (bOnGround || AirTimeAccumulator <= SprintAirTime);

    // 只有状态变化时才更新
    if (bSprintActive != bIsSprinting)
    {
        bIsSprinting = bSprintActive;
        MaxWalkSpeed = bIsSprinting ? SprintSpeed : WalkSpeed;
    }

}