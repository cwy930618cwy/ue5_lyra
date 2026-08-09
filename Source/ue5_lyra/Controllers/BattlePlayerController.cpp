#include "BattlePlayerController.h"
#include "Blueprint/UserWidget.h"

void ABattlePlayerController::BeginPlay()
{
    Super::BeginPlay();
    CreateHUD();
}

void ABattlePlayerController::CreateHUD()
{
    UE_LOG(LogTemp, Warning, TEXT("[HUD] CreateHUD 被调用"));

    // 运行时加载 WBP_HealthBar 蓝图类
    TSubclassOf<UUserWidget> HUDWidgetClass = LoadClass<UUserWidget>(
        nullptr,
        TEXT("/Game/MyResource/Blueprint/WBP_HealthBar.WBP_HealthBar_C"));
    if (!HUDWidgetClass)
    {
        UE_LOG(LogTemp, Error, TEXT("[HUD] LoadClass 失败！路径：/Game/MyResource/Blueprint/WBP_HealthBar.WBP_HealthBar_C"));
        return;
    }
    UE_LOG(LogTemp, Warning, TEXT("[HUD] LoadClass 成功"));

    UUserWidget* Widget = CreateWidget<UUserWidget>(this, HUDWidgetClass);
    if (!Widget)
    {
        UE_LOG(LogTemp, Error, TEXT("[HUD] CreateWidget 失败"));
        return;
    }
    UE_LOG(LogTemp, Warning, TEXT("[HUD] CreateWidget 成功"));

    Widget->AddToViewport();
    UE_LOG(LogTemp, Warning, TEXT("[HUD] AddToViewport 已执行"));
}