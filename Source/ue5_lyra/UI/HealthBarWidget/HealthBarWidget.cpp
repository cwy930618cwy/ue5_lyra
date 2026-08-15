#include "HealthBarWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UHealthBarWidget::SetHealth(float Current, float Max)
{
    // 1. 设置进度条的百分比
    if (HealthBar)
    {
        HealthBar->SetPercent(Max > 0.f ? Current / Max : 0.f);
    }
    // 2. 设置文本
    if (HealthText)
    {
        HealthText->SetText(FText::FromString(FString::Printf(TEXT("%.0f / %.0f"), Current, Max)));
    }
}