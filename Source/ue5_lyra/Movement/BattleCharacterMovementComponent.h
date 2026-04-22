// 角色移动组件 自定义
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BattleCharacterMovementComponent.generated.h"

UCLASS()
class UE5_LYRA_API UBattleCharacterMovementComponent : public UCharacterMovementComponent
{
    GENERATED_BODY()
public:
    UBattleCharacterMovementComponent();

    // 设置是否想要冲刺
    void SetWantsToSprint(bool bSprint);

    // 是否在冲刺
    bool IsSprinting() const;

    // 开始冲刺
    void StartSprint();

    // 停止冲刺
    void StopSprint();

protected:
    // 每帧更新，检查冲刺状态
    virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
    // 走路速度
    UPROPERTY(EditAnywhere, Category = "Sprint")
    float WalkSpeed = 500.0f;

    // 冲刺速度
    UPROPERTY(EditAnywhere, Category = "Sprint")
    float SprintSpeed = 3000.0f;

    // 是否想要冲刺
    bool bWantsToSprint = false;

    // 是否正在冲刺
    bool bIsSprinting = false;

};