#pragma once

#include "CoreMinimal.h"
#include "ABDCombatEnums.generated.h"

UENUM(BlueprintType)
enum class ECombatState : uint8
{
    Idle,           // 待机（可移动、可攻击）
    Moving,         // 移动中
    Attacking,      // 攻击中（不可打断）
    HitReacting,    // 受击硬直中
    Dead            // 死亡
};