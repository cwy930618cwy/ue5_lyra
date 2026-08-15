#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HealthBarWidget.generated.h"

class UProgressBar; // 进度条
class UTextBlock; // 文本框
class ABattleCharacter; // 角色

UCLASS()
class UE5_LYRA_API UHealthBarWidget : public UUserWidget
{
    GENERATED_BODY()
public:
    // 1.绑定血条控件
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UProgressBar> HealthBar;

    // 2.绑定文本控件
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> HealthText;

    // 3.刷新血量显示的函数
    UFUNCTION(BlueprintCallable)
    void SetHealth(float Current, float Max);

};