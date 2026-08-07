// 血量属性集cpp
#include "HealthSet.h"

UHealthSet::UHealthSet()
{
    InitHealth(100.0f);
    InitMaxHealth(100.0f);
}

// 属性即将变化时调用：把 Health 钳制在 [0, MaxHealth] 之间，防止加血溢出 / 扣血到负数
void UHealthSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
    Super::PreAttributeChange(Attribute, NewValue);

    if (Attribute == GetHealthAttribute())
    {
        NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxHealth());
    }
}