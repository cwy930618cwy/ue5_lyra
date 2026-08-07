#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayEffectTypes.h"
#include "HealthComponent.generated.h"

class UAbilitySystemComponent;
class UHealthSet;

UCLASS(classGroup = (Custom), meta = (BlueprintSpawnableComponent))
class UE5_LYRA_API UHealthComponent : public UActorComponent
{
    GENERATED_BODY()
public:
    UHealthComponent();
protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    void OnHealthChanged(const FOnAttributeChangeData& Data) const;

    // 持有 ASC 引用，后续注册/反注册回调都要用
    UPROPERTY()
    UAbilitySystemComponent* ASC;

    // 持有 HealthSet 引用，后续获取血量值都要用
    UPROPERTY()
    UHealthSet* HealthSet;

    // BattleCharacter.h public 区域加：
    UHealthSet* GetHealthSet() const { return HealthSet; }
};