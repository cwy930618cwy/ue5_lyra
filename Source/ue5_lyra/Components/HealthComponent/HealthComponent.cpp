#include "Components/HealthComponent/HealthComponent.h"
#include "AbilitySystemComponent.h"
#include "Attributes/HealthSet/HealthSet.h"
#include "Character/BattleCharacter.h"
#include "UI/HealthBarWidget/HealthBarWidget.h"

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

    // 打印血量变化信息
    UE_LOG(LogTemp, Warning, TEXT("Health changed: %f → %f (delta=%f)"), 
           OldHealth, NewHealth, Delta);

    // 受伤
    if (Delta < 0.0f)
    {
        UE_LOG(LogTemp, Warning, TEXT(">>> HURT! Took %f damage"), -Delta);
    }
    // 治疗
    else if (Delta > 0.0f)
    {
        UE_LOG(LogTemp, Warning, TEXT(">>> HEALED! Got %f healing"), Delta);
    }

    // 死亡检查
    if (NewHealth <= 0.0f && OldHealth > 0.0f)
    {
        UE_LOG(LogTemp, Error, TEXT("!!! DEAD !!!"));
    }

    if(HealthBarWidget) {
        HealthBarWidget->SetHealth(NewHealth, HealthSet->GetMaxHealth());
    }
}