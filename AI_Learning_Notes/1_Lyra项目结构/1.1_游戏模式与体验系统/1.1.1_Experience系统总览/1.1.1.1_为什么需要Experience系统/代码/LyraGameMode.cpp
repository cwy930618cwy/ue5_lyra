// Copyright Epic Games, Inc. All Rights Reserved.
// 
// 本文件对应的文档：01_为什么需要Experience系统.md
// 核心概念：GameMode 如何读取 Experience 配置来动态决定 Pawn 类型

#include "LyraGameMode.h"
#include "AssetRegistry/AssetData.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "LyraLogChannels.h"
#include "Misc/CommandLine.h"
#include "System/LyraAssetManager.h"
#include "LyraGameState.h"
#include "System/LyraGameSession.h"
#include "Player/LyraPlayerController.h"
#include "Player/LyraPlayerBotController.h"
#include "Player/LyraPlayerState.h"
#include "Character/LyraCharacter.h"
#include "UI/LyraHUD.h"
#include "Character/LyraPawnExtensionComponent.h"
#include "Character/LyraPawnData.h"
#include "GameModes/LyraWorldSettings.h"
#include "GameModes/LyraExperienceDefinition.h"
#include "GameModes/LyraExperienceManagerComponent.h"
#include "GameModes/LyraUserFacingExperienceDefinition.h"
#include "Kismet/GameplayStatics.h"
#include "Development/LyraDeveloperSettings.h"
#include "Player/LyraPlayerSpawningManagerComponent.h"
#include "CommonUserSubsystem.h"
#include "CommonSessionSubsystem.h"
#include "TimerManager.h"
#include "GameMapsSettings.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(LyraGameMode)

/**
 * 构造函数 — 指定所有默认类
 * 
 * 【注意】DefaultPawnClass 只是 fallback，实际 Pawn 类型会被 Experience → PawnData 覆盖
 */
ALyraGameMode::ALyraGameMode(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	GameStateClass = ALyraGameState::StaticClass();                    // 游戏状态
	GameSessionClass = ALyraGameSession::StaticClass();                // 游戏会话（在线匹配）
	PlayerControllerClass = ALyraPlayerController::StaticClass();      // 玩家控制器
	ReplaySpectatorPlayerControllerClass = ALyraReplayPlayerController::StaticClass();  // 回放观察者
	PlayerStateClass = ALyraPlayerState::StaticClass();                // 玩家状态（持有 ASC！）
	DefaultPawnClass = ALyraCharacter::StaticClass();                  // 默认 Pawn（会被 Experience 覆盖）
	HUDClass = ALyraHUD::StaticClass();                                // HUD
}

// =====================================================================
// GetPawnDataForController — 获取 Controller 对应的 PawnData
// =====================================================================
// 【查找顺序】
// 1. PlayerState 上是否已有 PawnData（已被赋值过）→ 直接返回
// 2. Experience 已加载 → 返回 Experience->DefaultPawnData
// 3. Experience 未加载 → 返回 nullptr（等加载完再处理）
const ULyraPawnData* ALyraGameMode::GetPawnDataForController(const AController* InController) const
{
	// 先检查 PlayerState（可能已经被赋过值）
	if (InController != nullptr)
	{
		if (const ALyraPlayerState* LyraPS = InController->GetPlayerState<ALyraPlayerState>())
		{
			if (const ULyraPawnData* PawnData = LyraPS->GetPawnData<ULyraPawnData>())
			{
				return PawnData;
			}
		}
	}

	// PlayerState 上没有，从 Experience 获取
	check(GameState);
	ULyraExperienceManagerComponent* ExperienceComponent = GameState->FindComponentByClass<ULyraExperienceManagerComponent>();
	check(ExperienceComponent);

	if (ExperienceComponent->IsExperienceLoaded())
	{
		const ULyraExperienceDefinition* Experience = ExperienceComponent->GetCurrentExperienceChecked();
		if (Experience->DefaultPawnData != nullptr)
		{
			return Experience->DefaultPawnData;  // 返回 Experience 配置的 PawnData！
		}

		// Experience 加载了但仍无 PawnData，用全局默认
		return ULyraAssetManager::Get().GetDefaultPawnData();
	}

	// Experience 还没加载完，暂时没有 PawnData
	return nullptr;
}

// =====================================================================
// InitGame — 游戏初始化
// =====================================================================
// 【关键】下一帧才调用 HandleMatchAssignmentIfNotExpectingOne()
// 【原因】需要等待启动设置初始化完成
void ALyraGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	// 等下一帧再确定 Experience（给时间让启动设置初始化）
	GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ThisClass::HandleMatchAssignmentIfNotExpectingOne);
}

// =====================================================================
// HandleMatchAssignmentIfNotExpectingOne — 确定使用哪个 Experience
// =====================================================================
// 【优先级从高到低】
// 1. URL Options（如 ?Experience=ShooterExperience）
// 2. Developer Settings（仅编辑器 PIE 模式）
// 3. 命令行（如 -Experience=ShooterExperience）
// 4. WorldSettings 里的默认 Experience
// 5. 专用服务器托管
// 6. Fallback：B_LyraDefaultExperience
void ALyraGameMode::HandleMatchAssignmentIfNotExpectingOne()
{
	FPrimaryAssetId ExperienceId;
	FString ExperienceIdSource;

	UWorld* World = GetWorld();

	// ① 检查 URL Options
	if (!ExperienceId.IsValid() && UGameplayStatics::HasOption(OptionsString, TEXT("Experience")))
	{
		const FString ExperienceFromOptions = UGameplayStatics::ParseOption(OptionsString, TEXT("Experience"));
		ExperienceId = FPrimaryAssetId(FPrimaryAssetType(ULyraExperienceDefinition::StaticClass()->GetFName()), FName(*ExperienceFromOptions));
		ExperienceIdSource = TEXT("OptionsString");
	}

	// ② 检查 Developer Settings（仅 PIE）
	if (!ExperienceId.IsValid() && World->IsPlayInEditor())
	{
		ExperienceId = GetDefault<ULyraDeveloperSettings>()->ExperienceOverride;
		ExperienceIdSource = TEXT("DeveloperSettings");
	}

	// ③ 检查命令行
	if (!ExperienceId.IsValid())
	{
		FString ExperienceFromCommandLine;
		if (FParse::Value(FCommandLine::Get(), TEXT("Experience="), ExperienceFromCommandLine))
		{
			ExperienceId = FPrimaryAssetId::ParseTypeAndName(ExperienceFromCommandLine);
			if (!ExperienceId.PrimaryAssetType.IsValid())
			{
				ExperienceId = FPrimaryAssetId(FPrimaryAssetType(ULyraExperienceDefinition::StaticClass()->GetFName()), FName(*ExperienceFromCommandLine));
			}
			ExperienceIdSource = TEXT("CommandLine");
		}
	}

	// ④ 检查 WorldSettings 里的默认 Experience
	if (!ExperienceId.IsValid())
	{
		if (ALyraWorldSettings* TypedWorldSettings = Cast<ALyraWorldSettings>(GetWorldSettings()))
		{
			ExperienceId = TypedWorldSettings->GetDefaultGameplayExperience();
			ExperienceIdSource = TEXT("WorldSettings");
		}
	}

	// 验证 Experience 是否存在
	ULyraAssetManager& AssetManager = ULyraAssetManager::Get();
	FAssetData Dummy;
	if (ExperienceId.IsValid() && !AssetManager.GetPrimaryAssetData(ExperienceId, /*out*/ Dummy))
	{
		UE_LOG(LogLyraExperience, Error, TEXT("EXPERIENCE: Wanted to use %s but couldn't find it, falling back to the default)"), *ExperienceId.ToString());
		ExperienceId = FPrimaryAssetId();  // 无效，继续 fallback
	}

	// ⑤ 最终 fallback
	if (!ExperienceId.IsValid())
	{
		// 尝试专用服务器登录
		if (TryDedicatedServerLogin())
		{
			return;
		}

		//@TODO: Pull this from a config setting or something
		ExperienceId = FPrimaryAssetId(FPrimaryAssetType("LyraExperienceDefinition"), FName("B_LyraDefaultExperience"));
		ExperienceIdSource = TEXT("Default");
	}

	// 确定了 Experience，通知管理器开始加载
	OnMatchAssignmentGiven(ExperienceId, ExperienceIdSource);
}

// =====================================================================
// TryDedicatedServerLogin — 尝试专用服务器登录
// =====================================================================
bool ALyraGameMode::TryDedicatedServerLogin()
{
	FString DefaultMap = UGameMapsSettings::GetGameDefaultMap();
	UWorld* World = GetWorld();
	UGameInstance* GameInstance = GetGameInstance();
	
	// 只在默认地图的专用服务器上注册
	if (GameInstance && World && World->GetNetMode() == NM_DedicatedServer && World->URL.Map == DefaultMap)
	{
		UCommonUserSubsystem* UserSubsystem = GameInstance->GetSubsystem<UCommonUserSubsystem>();
		UserSubsystem->OnUserInitializeComplete.AddDynamic(this, &ALyraGameMode::OnUserInitializedForDedicatedServer);

		// 专用服务器没有本地用户，索引 0 表示默认平台用户
		if (!UserSubsystem->TryToLoginForOnlinePlay(0))
		{
			OnUserInitializedForDedicatedServer(nullptr, false, FText(), ECommonUserPrivilege::CanPlayOnline, ECommonUserOnlineContext::Default);
		}

		return true;
	}

	return false;
}

// =====================================================================
// HostDedicatedServerMatch — 托管专用服务器比赛
// =====================================================================
void ALyraGameMode::HostDedicatedServerMatch(ECommonSessionOnlineMode OnlineMode)
{
	FPrimaryAssetType UserExperienceType = ULyraUserFacingExperienceDefinition::StaticClass()->GetFName();
	
	// 从命令行获取要托管的 UserFacingExperience
	FPrimaryAssetId UserExperienceId;
	FString UserExperienceFromCommandLine;
	if (FParse::Value(FCommandLine::Get(), TEXT("UserExperience="), UserExperienceFromCommandLine) ||
		FParse::Value(FCommandLine::Get(), TEXT("Playlist="), UserExperienceFromCommandLine))
	{
		UserExperienceId = FPrimaryAssetId::ParseTypeAndName(UserExperienceFromCommandLine);
		if (!UserExperienceId.PrimaryAssetType.IsValid())
		{
			UserExperienceId = FPrimaryAssetId(FPrimaryAssetType(UserExperienceType), FName(*UserExperienceFromCommandLine));
		}
	}

	// 强制加载所有 UserFacingExperience（专用服务器启动时可以阻塞）
	ULyraAssetManager& AssetManager = ULyraAssetManager::Get();
	TSharedPtr<FStreamableHandle> Handle = AssetManager.LoadPrimaryAssetsWithType(UserExperienceType);
	if (ensure(Handle.IsValid()))
	{
		Handle->WaitUntilComplete();
	}

	// 查找匹配的 Experience
	TArray<UObject*> UserExperiences;
	AssetManager.GetPrimaryAssetObjectList(UserExperienceType, UserExperiences);
	ULyraUserFacingExperienceDefinition* FoundExperience = nullptr;
	ULyraUserFacingExperienceDefinition* DefaultExperience = nullptr;

	for (UObject* Object : UserExperiences)
	{
		ULyraUserFacingExperienceDefinition* UserExperience = Cast<ULyraUserFacingExperienceDefinition>(Object);
		if (ensure(UserExperience))
		{
			if (UserExperience->GetPrimaryAssetId() == UserExperienceId)
			{
				FoundExperience = UserExperience;
				break;
			}
			
			// 记录默认 Experience 作为 fallback
			if (UserExperience->bIsDefaultExperience && DefaultExperience == nullptr)
			{
				DefaultExperience = UserExperience;
			}
		}
	}

	if (FoundExperience == nullptr)
	{
		FoundExperience = DefaultExperience;
	}
	
	UGameInstance* GameInstance = GetGameInstance();
	if (ensure(FoundExperience && GameInstance))
	{
		// 创建托管请求并启动会话
		UCommonSession_HostSessionRequest* HostRequest = FoundExperience->CreateHostingRequest(this);
		if (ensure(HostRequest))
		{
			HostRequest->OnlineMode = OnlineMode;

			UCommonSessionSubsystem* SessionSubsystem = GameInstance->GetSubsystem<UCommonSessionSubsystem>();
			SessionSubsystem->HostSession(nullptr, HostRequest);
		}
	}
}

// =====================================================================
// OnUserInitializedForDedicatedServer — 专用服务器用户初始化回调
// =====================================================================
void ALyraGameMode::OnUserInitializedForDedicatedServer(const UCommonUserInfo* UserInfo, bool bSuccess, FText Error, ECommonUserPrivilege RequestedPrivilege, ECommonUserOnlineContext OnlineContext)
{
	UGameInstance* GameInstance = GetGameInstance();
	if (GameInstance)
	{
		// 解绑回调（只用一次）
		UCommonUserSubsystem* UserSubsystem = GameInstance->GetSubsystem<UCommonUserSubsystem>();
		UserSubsystem->OnUserInitializeComplete.RemoveDynamic(this, &ALyraGameMode::OnUserInitializedForDedicatedServer);

		if (bSuccess && ensure(UserInfo))
		{
			UE_LOG(LogLyraExperience, Log, TEXT("Dedicated server user login succeeded for id %s, starting online server"), *UserInfo->GetNetId().ToString());
		}
		else
		{
			UE_LOG(LogLyraExperience, Log, TEXT("Dedicated server user login unsuccessful, starting online server as login is not required"));
		}
		
		HostDedicatedServerMatch(ECommonSessionOnlineMode::Online);
	}
}

// =====================================================================
// OnMatchAssignmentGiven — Experience 分配回调
// =====================================================================
// 【作用】找到 GameState 上的 ExperienceManagerComponent，调用 SetCurrentExperience
void ALyraGameMode::OnMatchAssignmentGiven(FPrimaryAssetId ExperienceId, const FString& ExperienceIdSource)
{
	if (ExperienceId.IsValid())
	{
		UE_LOG(LogLyraExperience, Log, TEXT("Identified experience %s (Source: %s)"), *ExperienceId.ToString(), *ExperienceIdSource);

		ULyraExperienceManagerComponent* ExperienceComponent = GameState->FindComponentByClass<ULyraExperienceManagerComponent>();
		check(ExperienceComponent);
		ExperienceComponent->SetCurrentExperience(ExperienceId);  // 触发加载流程！
	}
	else
	{
		UE_LOG(LogLyraExperience, Error, TEXT("Failed to identify experience, loading screen will stay up forever"));
	}
}

// =====================================================================
// OnExperienceLoaded — Experience 加载完成回调
// =====================================================================
// 【作用】为已经在场的玩家生成 Pawn
// （他们在 Experience 加载前就进来了，此时 Pawn 还是空的）
void ALyraGameMode::OnExperienceLoaded(const ULyraExperienceDefinition* CurrentExperience)
{
	// 遍历所有玩家控制器，为没有 Pawn 的生成一个
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		APlayerController* PC = Cast<APlayerController>(*Iterator);
		if ((PC != nullptr) && (PC->GetPawn() == nullptr))
		{
			if (PlayerCanRestart(PC))
			{
				RestartPlayer(PC);  // 生成 Pawn！
			}
		}
	}
}

/** 查询 Experience 是否已加载 */
bool ALyraGameMode::IsExperienceLoaded() const
{
	check(GameState);
	ULyraExperienceManagerComponent* ExperienceComponent = GameState->FindComponentByClass<ULyraExperienceManagerComponent>();
	check(ExperienceComponent);

	return ExperienceComponent->IsExperienceLoaded();
}

// =====================================================================
// GetDefaultPawnClassForController — 动态决定 Pawn 类型
// =====================================================================
// 【关键】由 Experience → PawnData → PawnClass 决定，而非硬编码
// 【这就是"同一套代码 + 不同配置 = 不同游戏模式"的核心实现！】
UClass* ALyraGameMode::GetDefaultPawnClassForController_Implementation(AController* InController)
{
	if (const ULyraPawnData* PawnData = GetPawnDataForController(InController))
	{
		if (PawnData->PawnClass)
		{
			return PawnData->PawnClass;  // 用 Experience 配置的 Pawn 类型！
		}
	}

	// Fallback：用构造函数里指定的 DefaultPawnClass
	return Super::GetDefaultPawnClassForController_Implementation(InController);
}

// =====================================================================
// SpawnDefaultPawnAtTransform — 生成 Pawn 并设置 PawnData
// =====================================================================
APawn* ALyraGameMode::SpawnDefaultPawnAtTransform_Implementation(AController* NewPlayer, const FTransform& SpawnTransform)
{
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.Instigator = GetInstigator();
	SpawnInfo.ObjectFlags |= RF_Transient;	// 不要把默认 Pawn 保存到地图里
	SpawnInfo.bDeferConstruction = true;       // 延迟构造，等设置完 PawnData 再完成

	if (UClass* PawnClass = GetDefaultPawnClassForController(NewPlayer))
	{
		if (APawn* SpawnedPawn = GetWorld()->SpawnActor<APawn>(PawnClass, SpawnTransform, SpawnInfo))
		{
			// 【关键】把 PawnData 设置到 PawnExtensionComponent
			if (ULyraPawnExtensionComponent* PawnExtComp = ULyraPawnExtensionComponent::FindPawnExtensionComponent(SpawnedPawn))
			{
				if (const ULyraPawnData* PawnData = GetPawnDataForController(NewPlayer))
				{
					PawnExtComp->SetPawnData(PawnData);  // 这样 Pawn 就知道自己的配置了
				}
				else
				{
					UE_LOG(LogLyra, Error, TEXT("Game mode was unable to set PawnData on the spawned pawn [%s]."), *GetNameSafe(SpawnedPawn));
				}
			}

			SpawnedPawn->FinishSpawning(SpawnTransform);  // 完成构造

			return SpawnedPawn;
		}
		else
		{
			UE_LOG(LogLyra, Error, TEXT("Game mode was unable to spawn Pawn of class [%s] at [%s]."), *GetNameSafe(PawnClass), *SpawnTransform.ToHumanReadableString());
		}
	}
	else
	{
		UE_LOG(LogLyra, Error, TEXT("Game mode was unable to spawn Pawn due to NULL pawn class."));
	}

	return nullptr;
}

/** Lyra 不使用引擎默认的出生点，完全由 PlayerSpawningManagerComponent 管理 */
bool ALyraGameMode::ShouldSpawnAtStartSpot(AController* Player)
{
	return false;  // 永远不用 start spot
}

// =====================================================================
// HandleStartingNewPlayer — 等 Experience 加载完再生成
// =====================================================================
void ALyraGameMode::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
	// 【关键】如果 Experience 还没加载完，就不生成 Pawn
	// 等 OnExperienceLoaded 回调再处理
	if (IsExperienceLoaded())
	{
		Super::HandleStartingNewPlayer_Implementation(NewPlayer);
	}
}

/** 选择玩家出生点（交给 PlayerSpawningManagerComponent） */
AActor* ALyraGameMode::ChoosePlayerStart_Implementation(AController* Player)
{
	if (ULyraPlayerSpawningManagerComponent* PlayerSpawningComponent = GameState->FindComponentByClass<ULyraPlayerSpawningManagerComponent>())
	{
		return PlayerSpawningComponent->ChoosePlayerStart(Player);
	}
	
	return Super::ChoosePlayerStart_Implementation(Player);
}

/** 完成玩家重生 */
void ALyraGameMode::FinishRestartPlayer(AController* NewPlayer, const FRotator& StartRotation)
{
	if (ULyraPlayerSpawningManagerComponent* PlayerSpawningComponent = GameState->FindComponentByClass<ULyraPlayerSpawningManagerComponent>())
	{
		PlayerSpawningComponent->FinishRestartPlayer(NewPlayer, StartRotation);
	}

	Super::FinishRestartPlayer(NewPlayer, StartRotation);
}

/** 判断玩家是否可以重生 */
bool ALyraGameMode::PlayerCanRestart_Implementation(APlayerController* Player)
{
	return ControllerCanRestart(Player);
}

/** 判断控制器是否可以重生（不区分玩家和 Bot） */
bool ALyraGameMode::ControllerCanRestart(AController* Controller)
{
	if (APlayerController* PC = Cast<APlayerController>(Controller))
	{	
		if (!Super::PlayerCanRestart_Implementation(PC))
		{
			return false;
		}
	}
	else
	{
		// Bot 版本的检查
		if ((Controller == nullptr) || Controller->IsPendingKillPending())
		{
			return false;
		}
	}

	if (ULyraPlayerSpawningManagerComponent* PlayerSpawningComponent = GameState->FindComponentByClass<ULyraPlayerSpawningManagerComponent>())
	{
		return PlayerSpawningComponent->ControllerCanRestart(Controller);
	}

	return true;
}

// =====================================================================
// InitGameState — 初始化 GameState（注册 Experience 回调）
// =====================================================================
void ALyraGameMode::InitGameState()
{
	Super::InitGameState();

	// 【关键】注册 OnExperienceLoaded 回调（普通优先级）
	ULyraExperienceManagerComponent* ExperienceComponent = GameState->FindComponentByClass<ULyraExperienceManagerComponent>();
	check(ExperienceComponent);
	ExperienceComponent->CallOrRegister_OnExperienceLoaded(FOnLyraExperienceLoaded::FDelegate::CreateUObject(this, &ThisClass::OnExperienceLoaded));
}

/** 通用玩家初始化（广播委托通知其他系统） */
void ALyraGameMode::GenericPlayerInitialization(AController* NewPlayer)
{
	Super::GenericPlayerInitialization(NewPlayer);

	OnGameModePlayerInitialized.Broadcast(this, NewPlayer);  // 广播"玩家初始化完成"事件
}

// =====================================================================
// RequestPlayerRestartNextFrame — 下帧重生玩家
// =====================================================================
void ALyraGameMode::RequestPlayerRestartNextFrame(AController* Controller, bool bForceReset)
{
	if (bForceReset && (Controller != nullptr))
	{
		Controller->Reset();  // 立即重置（放弃当前 Pawn）
	}

	if (APlayerController* PC = Cast<APlayerController>(Controller))
	{
		GetWorldTimerManager().SetTimerForNextTick(PC, &APlayerController::ServerRestartPlayer_Implementation);
	}
	else if (ALyraPlayerBotController* BotController = Cast<ALyraPlayerBotController>(Controller))
	{
		GetWorldTimerManager().SetTimerForNextTick(BotController, &ALyraPlayerBotController::ServerRestartController);
	}
}

/** 更新玩家出生点（现在什么都不做，等到 PostLogin 再生成） */
bool ALyraGameMode::UpdatePlayerStartSpot(AController* Player, const FString& Portal, FString& OutErrorMessage)
{
	return true;
}

// =====================================================================
// FailedToRestartPlayer — 重生失败处理
// =====================================================================
// 【逻辑】如果有 PawnClass 就继续尝试，否则放弃
void ALyraGameMode::FailedToRestartPlayer(AController* NewPlayer)
{
	Super::FailedToRestartPlayer(NewPlayer);

	// 检查是否有 PawnClass 可用
	if (UClass* PawnClass = GetDefaultPawnClassForController(NewPlayer))
	{
		if (APlayerController* NewPC = Cast<APlayerController>(NewPlayer))
		{
			// 玩家：检查是否还能重生（避免无限循环）
			if (PlayerCanRestart(NewPC))
			{
				RequestPlayerRestartNextFrame(NewPlayer, false);			
			}
			else
			{
				UE_LOG(LogLyra, Verbose, TEXT("FailedToRestartPlayer(%s) and PlayerCanRestart returned false, so we're not going to try again."), *GetPathNameSafe(NewPlayer));
			}
		}
		else
		{
			// Bot：继续尝试
			RequestPlayerRestartNextFrame(NewPlayer, false);
		}
	}
	else
	{
		UE_LOG(LogLyra, Verbose, TEXT("FailedToRestartPlayer(%s) but there's no pawn class so giving up."), *GetPathNameSafe(NewPlayer));
	}
}
