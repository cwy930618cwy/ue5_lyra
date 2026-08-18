#include "Components/DebugHelper/DebugHelper.h"
#include "Engine/Engine.h"

void UDebugHelper::PrintOnScreen(const FString& Message, float Duration, FColor Color)
{
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, Duration, Color, Message);
    }
}

void UDebugHelper::DebugLog(const FString& Message, float ScreenDuration, FColor Color)
{
    UE_LOG(LogTemp, Warning, TEXT("%s"), *Message);
    PrintOnScreen(Message, ScreenDuration, Color);
}
