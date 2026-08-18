#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "DebugHelper.generated.h"

// 调试工具函数库（蓝图也能用）
UCLASS()
class UE5_LYRA_API UDebugHelper : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    // 屏幕打印文字
    UFUNCTION(BlueprintCallable, Category = "Debug")
    static void PrintOnScreen(const FString& Message, float Duration = 5.0f, FColor Color = FColor::Yellow);

    // 屏幕打印 + 同时输出到日志
    UFUNCTION(BlueprintCallable, Category = "Debug")
    static void DebugLog(const FString& Message, float ScreenDuration = 5.0f, FColor Color = FColor::Yellow);
};
