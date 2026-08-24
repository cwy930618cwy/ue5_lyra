// 攻击技能
#pragma once

#include "CoreMinimal.h"
#include "../GA_BattleAbility/GA_BattleAbility.h"
#include "GA_Attack.generated.h"

/**
 * 攻击技能
 * - 激活时播放攻击蒙太奇
 * - 蒙太奇结束后调用 EndAbility
 */
UCLASS()
class UE5_LYRA_API UGA_Attack : public UGA_BattleAbility
{
    GENERATED_BODY()

public:
    UGA_Attack();

    // 技能激活时调用（你的逻辑写这里）
    virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

    // 技能结束时调用（清理工作）
    virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

protected:
    // 攻击蒙太奇（技能自带资源，不从角色偷）
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
    TObjectPtr<UAnimMontage> AttackMontage;

    // 蒙太奇播放结束回调（PlayMontageAndWait 的 OnCompleted 绑定到这里）
    UFUNCTION()
    void OnMontageFinished();
};
