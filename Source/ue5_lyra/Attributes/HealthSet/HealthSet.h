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

    // 攻击力
    UPROPERTY(BlueprintReadOnly, Category = "Combat")
    FGameplayAttributeData AttackPower;
    ATTRIBUTE_ACCESSORS(UHealthSet, AttackPower)

    // 防御力
    UPROPERTY(BlueprintReadOnly, Category = "Combat")
    FGameplayAttributeData Defense;
    ATTRIBUTE_ACCESSORS(UHealthSet, Defense)

    // 当前体力
    UPROPERTY(BlueprintReadOnly, Category = "Stamina")
    FGameplayAttributeData Stamina;
    ATTRIBUTE_ACCESSORS(UHealthSet, Stamina)

    // 最大体力
    UPROPERTY(BlueprintReadOnly, Category = "Stamina")
    FGameplayAttributeData MaxStamina;
    ATTRIBUTE_ACCESSORS(UHealthSet, MaxStamina)

    // 移动速度加成（1.0 = 100%，1.5 = 150%）
    UPROPERTY(BlueprintReadOnly, Category = "Combat")
    FGameplayAttributeData MoveSpeed;
    ATTRIBUTE_ACCESSORS(UHealthSet, MoveSpeed)

    // 属性即将变化时调用，用于把属性钳制在合理范围内
    virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
};