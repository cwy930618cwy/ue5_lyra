// 血量属性集cpp
#include "HealthSet.h"

UHealthSet::UHealthSet()
{
    InitHealth(100.0f);
    InitMaxHealth(100.0f);
    
    // 新增
    InitAttackPower(10.0f);
    InitDefense(5.0f);
    InitStamina(100.0f);
    InitMaxStamina(100.0f);
    InitMoveSpeed(1.0f);
}

// 属性即将变化时调用：把 Health 钳制在 [0, MaxHealth] 之间，防止加血溢出 / 扣血到负数
void UHealthSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
    Super::PreAttributeChange(Attribute, NewValue);

    if (Attribute == GetHealthAttribute())
    {
        NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxHealth());
    }
    else if (Attribute == GetStaminaAttribute())
    {
        NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxStamina());
    }
}