#include "Components/HealthComponent/HealthComponent.h"
#include "AbilitySystemComponent.h"
#include "Attributes/HealthSet/HealthSet.h"
#include "Character/BattleCharacter.h"
#include "UI/HealthBarWidget/HealthBarWidget.h"
#include "Components/DebugHelper/DebugHelper.h"

UHealthComponent::UHealthComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UHealthComponent::BeginPlay()
{
    Super::BeginPlay();
    
    // 获取组件挂载的 Actor
    AActor* Owner = GetOwner();
    if(!Owner) return;

    // 从 Actor 上找到 ASC（GAS 总调度中心）
    ASC = Owner->FindComponentByClass<UAbilitySystemComponent>();
    if(!ASC) return;

    ABattleCharacter* Character = Cast<ABattleCharacter>(Owner);
    if(!Character) return;

    // 缓存角色指针
    OwnerCharacter = Character;

    HealthSet = Character->GetHealthSet();
    if(!HealthSet) return;

    // 注册 CurHealth 属性变化回调
    // GetGameplayAttributeValueChangeDelegate 返回一个委托(Delegate)
    // AddUObject 把回调绑定到当前组件的 OnHealthChanged 方法
    ASC->GetGameplayAttributeValueChangeDelegate(
        HealthSet->GetHealthAttribute()
    ).AddUObject(this, &UHealthComponent::OnHealthChanged);
}

void UHealthComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);
    
    // 移除 CurHealth 属性变化回调
    if(ASC && HealthSet)
    {
        ASC->GetGameplayAttributeValueChangeDelegate(
            HealthSet->GetHealthAttribute()
        ).RemoveAll(this);
    }
}

void UHealthComponent::OnHealthChanged(const FOnAttributeChangeData& Data)
{
    float NewHealth = Data.NewValue;
    float OldHealth = Data.OldValue;
    float Delta = NewHealth - OldHealth;

    // 受伤
    if (Delta < 0.0f)
    {
        UDebugHelper::DebugLog(
            FString::Printf(TEXT(">>> HURT! Took %.1f damage"), -Delta),
            3.0f, FColor::Red
        );

        // 进入受击僵直
        if(OwnerCharacter) {
            OwnerCharacter->SetCombatState(ECombatState::HitReacting);

            // 延迟 0.5 秒后退出受击僵直
            FTimerHandle HitStunTimerHandle;
            GetWorld()->GetTimerManager().SetTimer(
                HitStunTimerHandle,  // ← 改成一样的
                this,
                &UHealthComponent::OnHitReactEnd,
                0.5f,
                false
            );
        }
    }
    // 治疗
    else if (Delta > 0.0f)
    {
        UDebugHelper::DebugLog(
            FString::Printf(TEXT(">>> HEALED! Got %.1f healing"), Delta),
            3.0f, FColor::Green
        );
    }

    // 死亡检查
    if (NewHealth <= 0.0f && OldHealth > 0.0f)
    {
        UDebugHelper::DebugLog(TEXT("!!! DEAD !!!"), 5.0f, FColor::Red);
    }

    if(HealthBarWidget) {
        HealthBarWidget->SetHealth(NewHealth, HealthSet->GetMaxHealth());
    }
}

void UHealthComponent::OnHitReactEnd()
{
    if(OwnerCharacter) {
        OwnerCharacter->SetCombatState(ECombatState::Idle);
    }
}