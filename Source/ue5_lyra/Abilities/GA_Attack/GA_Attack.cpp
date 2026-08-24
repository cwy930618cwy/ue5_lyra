// 攻击技能实现
#include "GA_Attack.h"
#include "GameFramework/Character.h"
#include "Animation/AnimMontage.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "Components/DebugHelper/DebugHelper.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "AbilitySystemBlueprintLibrary.h"

UGA_Attack::UGA_Attack()
{
    // 设置触发方式：收到 GameplayEvent 时触发
    FAbilityTriggerData TriggerData;
    TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
    AbilityTriggers.Add(TriggerData);
}

void UGA_Attack::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

    // 获取角色（AvatarActor 就是技能的施法者）
    ACharacter* Character = Cast<ACharacter>(ActorInfo->AvatarActor.Get());
    if (!Character)
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }

    // 用技能自带的蒙太奇（不从角色偷）
    if (!AttackMontage)
    {
        UDebugHelper::DebugLog(TEXT("[GA_Attack] AttackMontage 未配置！"), 2.0f, FColor::Red);
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }

    // 用 PlayMontageAndWait 播蒙太奇，播完自动回调 OnMontageFinished
    UAbilityTask_PlayMontageAndWait* Task = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
        this,                      // OwningAbility
        TEXT("AttackMontage"),     // TaskInstanceName
        AttackMontage,             // 要播的蒙太奇
        1.0f                       // Rate
    );
    if (Task)
    {
        Task->OnCompleted.AddDynamic(this, &UGA_Attack::OnMontageFinished);
        Task->ReadyForActivation();
    }
}

void UGA_Attack::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
    UDebugHelper::DebugLog(TEXT("[GA_Attack] 攻击技能结束"), 2.0f, FColor::Yellow);

    // 广播"攻击结束"事件，让角色复位状态（走 GameplayEvent，不 Cast 角色）
    if (ActorInfo && ActorInfo->AbilitySystemComponent.IsValid())
    {
        FGameplayEventData EventData;
        UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
            ActorInfo->AvatarActor.Get(),
            FGameplayTag::RequestGameplayTag(FName("Event.Combat.AttackEnded")),
            EventData
        );
    }

    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_Attack::OnMontageFinished()
{
    // 蒙太奇播完，结束技能（状态由此回到 Idle）
    EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, true);
}