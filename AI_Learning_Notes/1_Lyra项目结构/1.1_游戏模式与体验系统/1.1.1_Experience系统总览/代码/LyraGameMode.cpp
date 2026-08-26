// Copyright Epic Games, Inc. All Rights Reserved.

#include "LyraGameMode.h"
#include "LyraGameState.h"
#include "LyraExperienceManagerComponent.h"
#include "LyraExperienceDefinition.h"
#include "LyraPawnData.h"
#include "LyraLogChannels.h"

ALyraGameMode::ALyraGameMode(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

// 游戏初始化入口：比 BeginPlay 更早调用，是触发 Experience 加载的最佳时机
void ALyraGameMode::InitGame(const FURL& URL, UGameInstance* GameInstance)
{
	Super::InitGame(URL, GameInstance);

	// 触发 Experience 加载流程
	HandleMatchAssignmentIfNotExpectingOne();
}

// 按优先级确定用哪个 Experience：WorldSettings > 命令行 > 项目设置
void ALyraGameMode::HandleMatchAssignmentIfNotExpectingOne()
{
	// 已加载则跳过（防止重复加载）
	if (CurrentExperience)
	{
		return;
	}

	// 从 GameState 上获取 ExperienceManagerComponent
	// Component 挂在 GameState 上而不是 GameMode 上，因为 GameState 在客户端也存在
	if (ALyraGameState* GameState = GetGameState<ALyraGameState>())
	{
		if (ULyraExperienceManagerComponent* Component = GameState->FindComponentByClass<ULyraExperienceManagerComponent>())
		{
			// 委托给 Component 异步加载 Experience
			Component->SetCurrentExperience(FPrimaryAssetId());
		}
	}
}

// Experience 加载完成回调：广播委托 + 为在场玩家生成 Pawn
void ALyraGameMode::OnExperienceLoaded(const ULyraExperienceDefinition* Experience)
{
	// 保存当前 Experience 引用
	CurrentExperience = Experience;

	// 广播委托——其他系统（如 BotCreationComponent）可以监听这个事件
	OnExperienceLoadedDelegate.Broadcast(Experience);

	// 为所有已经存在的玩家生成 Pawn
	// 注意：此时玩家可能已经连接但还没有角色（在 Experience 加载期间连入的玩家）
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		APlayerController* PC = It->Get();
		if (PC && !PC->GetPawn())  // 只给没有 Pawn 的玩家生成
		{
			SpawnDefaultPawnAtTransform(PC, PC->GetSpawnLocation());
		}
	}
}

// 动态决定 Pawn 类型：从 Experience 配置的 PawnData 中获取
UClass* ALyraGameMode::GetDefaultPawnClassForController_Implementation(AController* InController)
{
	// 从 Experience 配置中查找 PawnData
	if (const ULyraPawnData* PawnData = GetPawnDataForController(InController))
	{
		if (PawnData->PawnClass)
		{
			// 用 Experience 配置的 Pawn 类型！这就是"数据驱动"的核心体现
			return PawnData->PawnClass;
		}
	}

	// 回退到父类默认行为（如果 Experience 没配置 PawnData）
	return Super::GetDefaultPawnClassForController_Implementation(InController);
}

// 查找链：从当前 Experience 的 DefaultPawnData 字段获取 Pawn 数据
ULyraPawnData* ALyraGameMode::GetPawnDataForController(AController* InController) const
{
	// 优先用 Experience 配置的 DefaultPawnData
	if (CurrentExperience && CurrentExperience->DefaultPawnData)
	{
		return const_cast<ULyraPawnData*>(CurrentExperience->DefaultPawnData.Get());
	}

	// TODO: 后续可以通过 PlayerState 上的 PawnDataComponent 覆盖（支持同一局游戏中不同玩家用不同角色）
	return nullptr;
}

// 生成 Pawn：用 Experience 配置的 Pawn 类型在指定位置生成
APawn* ALyraGameMode::SpawnDefaultPawnAtTransform_Implementation(AController* NewPlayer, const FTransform& SpawnTransform)
{
	UClass* PawnClass = GetDefaultPawnClassForController(NewPlayer);
	if (PawnClass)
	{
		// 用 Experience 配置的 Pawn 类型生成角色
		return GetWorld()->SpawnActor<APawn>(PawnClass, SpawnTransform);
	}

	// 回退到父类默认行为
	return Super::SpawnDefaultPawnAtTransform_Implementation(NewPlayer, SpawnTransform);
}

// 新玩家加入时的处理（Experience 未就绪时会在这里等待）
void ALyraGameMode::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
	Super::HandleStartingNewPlayer_Implementation(NewPlayer);
}
