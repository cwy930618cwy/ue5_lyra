// 血量属性集
#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"  // 包含 AttributeSet 基类
#include "AbilitySystemComponent.h"  // 包含 AbilitySystemComponent
#include "HealthSet.generated.h"

// 一次性生成 4 个静态函数：GetXxxAttribute / GetXxx / SetXxx / InitXxx
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
    GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

UCLASS()
class UE5_LYRA_API UHealthSet : public UAttributeSet
{
    GENERATED_BODY()
public:
    UHealthSet();

    // 当前血量
    UPROPERTY(BlueprintReadOnly, Category = "Health")
    FGameplayAttributeData Health;
    ATTRIBUTE_ACCESSORS(UHealthSet, Health)

    // 最大血量
    UPROPERTY(BlueprintReadOnly, Category = "Health")
    FGameplayAttributeData MaxHealth;
    ATTRIBUTE_ACCESSORS(UHealthSet, MaxHealth)

    // 属性即将变化时调用，用于把属性钳制在合理范围内
    virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
};