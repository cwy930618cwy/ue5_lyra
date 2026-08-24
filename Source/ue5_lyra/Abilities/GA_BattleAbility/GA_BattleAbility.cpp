// GA 基类实现
#include "GA_BattleAbility.h"

UGA_BattleAbility::UGA_BattleAbility()
{
    // 每个角色一个实例（Lyra 官方做法，攻击/跳跃/交互都用这个）
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

    // 单机项目用 LocalPredicted（客户端先预测执行）
    NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}
