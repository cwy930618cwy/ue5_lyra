#include "BattlePlayerController.h"
#include "Blueprint/UserWidget.h"
#include "UI/HealthBarWidget/HealthBarWidget.h"
#include "Character/BattleCharacter.h"
#include "Attributes/HealthSet/HealthSet.h"   // ← 关键！这个有没有加？

void ABattlePlayerController::BeginPlay()
{
    Super::BeginPlay();
    CreateHUD();
}

void ABattlePlayerController::CreateHUD()
{
    // 加载血条控件蓝图类
    TSubclassOf<UHealthBarWidget> HUDWidgetClass = LoadClass<UHealthBarWidget>(
        nullptr,
        TEXT("/Game/MyResource/Blueprint/WBP_HealthBar.WBP_HealthBar_C"));
    if (!HUDWidgetClass) return;

    // 创建血条控件实例（注意模板类型改成了 UHealthBarWidget）
    HealthBarWidget = CreateWidget<UHealthBarWidget>(this, HUDWidgetClass);
    if (HealthBarWidget)
    {
        HealthBarWidget->AddToViewport();

        // 从当前角色读血量并刷新
        if (ABattleCharacter* BattleChar = Cast<ABattleCharacter>(GetPawn()))
        {
            HealthBarWidget->SetHealth(
                BattleChar->GetHealthSet()->GetHealth(),
                BattleChar->GetHealthSet()->GetMaxHealth());
        }
    }
}