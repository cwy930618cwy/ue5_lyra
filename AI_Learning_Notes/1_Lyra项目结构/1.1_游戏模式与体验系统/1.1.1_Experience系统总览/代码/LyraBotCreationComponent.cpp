// Copyright Epic Games, Inc. All Rights Reserved.

#include "LyraBotCreationComponent.h"
#include "LyraGameState.h"
#include "LyraExperienceManagerComponent.h"
#include "LyraExperienceDefinition.h"
#include "LyraLogChannels.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(LyraBotCreationComponent)

ULyraBotCreationComponent::ULyraBotCreationComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

// BeginPlay：注册 Experience 加载完成回调
void ULyraBotCreationComponent::BeginPlay()
{
	Super::BeginPlay();

	// 找到 GameState 上的 ExperienceManagerComponent
	if (ALyraGameState* GS = GetGameState<ALyraGameState>())
	{
		if (ULyraExperienceManagerComponent* ExpMgrComp = GS->GetExperienceManagerComponent())
		{
			// 注册低优先级回调（在 GameMode/Pawn 初始化之后再创建 Bot）
			ExpMgrComp->CallOrRegister_OnExperienceLoaded_LowPriority(
				FOnLyraExperienceLoaded::FDelegate::CreateUObject(this, &ThisClass::OnExperienceLoaded));
		}
	}
}

// EndPlay：清理所有 Bot
void ULyraBotCreationComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// 销毁所有已创建的 Bot
	for (AController* BotController : CreatedBots)
	{
		if (BotController)
		{
			BotController->Destroy();
		}
	}
	CreatedBots.Empty();

	Super::EndPlay(EndPlayReason);
}

// Experience 加载完成回调 → 按配置数量创建 Bot
void ULyraBotCreationComponent::OnExperienceLoaded(const ULyraExperienceDefinition* Experience)
{
	UE_LOG(LogLyraExperience, Log, TEXT("BotCreationComponent: Experience loaded, creating %d bots"), NumBotsToCreate);

	// 按配置数量逐个创建 Bot
	for (int32 i = 0; i < NumBotsToCreate; i++)
	{
		SpawnOneBot();
	}
}

// 创建单个 Bot（可由子类覆写自定义生成逻辑）
void ULyraBotCreationComponent::SpawnOneBot()
{
	AController* NewBotController = nullptr;

	// 获取 PlayerStart 位置作为 Bot 的出生点
	FTransform SpawnTransform = FTransform::Identity;
	if (APlayerStart* StartPoint = UGameplayStatics::GetPlayerStart(GetWorld(), 0))
	{
		SpawnTransform = StartPoint->GetActorTransform();
	}

	// 生成 Bot 控制器
	if (BotControllerClass)
	{
		NewBotController = GetWorld()->SpawnActor<AController>(BotControllerClass, SpawnTransform);
	}

	if (NewBotController)
	{
		// 记录到已创建列表
		CreatedBots.Add(NewBotController);

		// 让 Bot 拥有自己的 Pawn
		if (BotPawnClass)
		{
			APawn* BotPawn = GetWorld()->SpawnActor<APawn>(BotPawnClass, SpawnTransform);
			NewBotController->Possess(BotPawn);
		}
	}
}

// 移除单个 Bot
void ULyraBotCreationComponent::RemoveOneBot()
{
	if (CreatedBots.Num() > 0)
	{
		// 移除最后一个创建的 Bot
		AController* BotToRemove = CreatedBots.Pop();
		if (BotToRemove)
		{
			BotToRemove->Destroy();
		}
	}
}
