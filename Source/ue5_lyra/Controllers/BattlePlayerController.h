#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BattlePlayerController.generated.h"

UCLASS()
class UE5_LYRA_API ABattlePlayerController : public APlayerController
{
    GENERATED_BODY()

public:
    // 游戏开始时自动调用，在这里创建UI
    virtual void BeginPlay() override;

protected:
    // 创建血条UI并添加到屏幕
    UFUNCTION()
    void CreateHUD();
};