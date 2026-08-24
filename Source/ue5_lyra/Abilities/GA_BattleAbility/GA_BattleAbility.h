// 技能基类
#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GA_BattleAbility.generated.h"

/**
 * 所有战斗技能的基类
 * - 统一 InstancingPolicy = PerExecution（每次激活新建实例）
 * - 统一 NetExecutionPolicy = LocalPredicted（单机预测）
 */
UCLASS()
class UE5_LYRA_API UGA_BattleAbility : public UGameplayAbility
{
    GENERATED_BODY()

public:
    UGA_BattleAbility();
};
