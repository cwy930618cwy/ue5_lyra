// Copyright Epic Games, Inc. All Rights Reserved.
// 
// 本文件对应的文档：01_为什么需要Experience系统.md
// 核心概念：GameMode 如何读取 Experience 配置来动态决定 Pawn 类型

#pragma once

#include "ModularGameMode.h"              // 模块化 GameMode 基类（支持组件化）

#include "LyraGameMode.generated.h"

class AActor;
class AController;
class AGameModeBase;
class APawn;
class APlayerController;
class UClass;
class ULyraExperienceDefinition;         // 体验定义 → 详见 LyraExperienceDefinition.h
class ULyraPawnData;                     // Pawn 数据 → 定义角色配置
class UObject;
struct FFrame;
struct FPrimaryAssetId;
enum class ECommonSessionOnlineMode : uint8;

/**
 * 玩家初始化完成事件委托
 */
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnLyraGameModePlayerInitialized, AGameModeBase* /*GameMode*/, AController* /*NewPlayer*/);

/**
 * ============================================
 * Lyra 游戏模式基类
 * ============================================
 * 
 * 【一句话概括】
 * Lyra 的游戏模式，负责玩家登录、Pawn 生成、Experience 选择。
 * 
 * 【与传统 GameMode 的区别】
 * 传统：DefaultPawnClass 硬编码在构造函数里
 * Lyra：Pawn 类型由 Experience → PawnData → PawnClass 动态决定
 * 
 * 【核心方法】
 * - GetPawnDataForController(): 从 Experience 获取 PawnData
 * - GetDefaultPawnClassForController(): 根据 PawnData 返回 PawnClass
 * - SpawnDefaultPawnAtTransform(): 生成 Pawn 并设置 PawnData
 */
UCLASS(Config = Game, Meta = (ShortTooltip = "The base game mode class used by this project."))
class LYRAGAME_API ALyraGameMode : public AModularGameModeBase
{
	GENERATED_BODY()

public:

	ALyraGameMode(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	// =====================================================================
	// 获取指定 Controller 对应的 PawnData
	// =====================================================================
	// 【查找顺序】
	// 1. PlayerState 上是否已有 PawnData（已被赋值过）→ 直接返回
	// 2. Experience 已加载 → 返回 Experience->DefaultPawnData
	// 3. Experience 未加载 → 返回 nullptr（等加载完再处理）
	UFUNCTION(BlueprintCallable, Category = "Lyra|Pawn")
	const ULyraPawnData* GetPawnDataForController(const AController* InController) const;

	//~AGameModeBase interface
	
	/** 游戏初始化（下一帧调用 HandleMatchAssignmentIfNotExpectingOne） */
	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
	
	/** 
	 * 获取默认 Pawn 类型
	 * 【关键】由 Experience → PawnData → PawnClass 决定，而非硬编码
	 */
	virtual UClass* GetDefaultPawnClassForController_Implementation(AController* InController) override;
	
	/** 
	 * 在指定位置生成 Pawn
	 * 【关键】生成后会把 PawnData 设置到 PawnExtensionComponent
	 */
	virtual APawn* SpawnDefaultPawnAtTransform_Implementation(AController* NewPlayer, const FTransform& SpawnTransform) override;
	
	/** Lyra 不用引擎默认的出生点，完全由 PlayerSpawningManagerComponent 管理 */
	virtual bool ShouldSpawnAtStartSpot(AController* Player) override;
	
	/** 
	 * 处理新玩家
	 * 【关键】如果 Experience 未加载则等待（避免数据未就绪就生成 Pawn）
	 */
	virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;
	
	/** 选择玩家出生点 */
	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;
	
	/** 完成玩家重生 */
	virtual void FinishRestartPlayer(AController* NewPlayer, const FRotator& StartRotation) override;
	
	/** 判断玩家是否可以重生 */
	virtual bool PlayerCanRestart_Implementation(APlayerController* Player) override;
	
	/** 初始化 GameState（注册 OnExperienceLoaded 回调） */
	virtual void InitGameState() override;
	
	/** 更新玩家出生点 */
	virtual bool UpdatePlayerStartSpot(AController* Player, const FString& Portal, FString& OutErrorMessage) override;
	
	/** 通用玩家初始化 */
	virtual void GenericPlayerInitialization(AController* NewPlayer) override;
	
	/** 重生失败处理（有 PawnClass 就继续尝试） */
	virtual void FailedToRestartPlayer(AController* NewPlayer) override;
	//~End of AGameModeBase interface

	// =====================================================================
	// 下帧重生指定玩家或 Bot
	// =====================================================================
	UFUNCTION(BlueprintCallable)
	void RequestPlayerRestartNextFrame(AController* Controller, bool bForceReset = false);

	// =====================================================================
	// 判断控制器是否可以重生（不区分玩家和 Bot）
	// =====================================================================
	virtual bool ControllerCanRestart(AController* Controller);

	// =====================================================================
	// 玩家初始化完成委托
	// =====================================================================
	FOnLyraGameModePlayerInitialized OnGameModePlayerInitialized;

protected:	
	// =====================================================================
	// Experience 加载完成回调
	// =====================================================================
	// 【作用】为已经在场的玩家生成 Pawn
	// （他们在 Experience 加载前就进来了，此时 Pawn 还是空的）
	void OnExperienceLoaded(const ULyraExperienceDefinition* CurrentExperience);
	
	/** 查询 Experience 是否已加载 */
	bool IsExperienceLoaded() const;

	// =====================================================================
	// Experience 分配回调
	// =====================================================================
	// 【调用时机】确定了用哪个 Experience 后
	void OnMatchAssignmentGiven(FPrimaryAssetId ExperienceId, const FString& ExperienceIdSource);

	// =====================================================================
	// 确定 Experience（按优先级检查多个来源）
	// =====================================================================
	// 【优先级从高到低】
	// 1. URL Options（?Experience=xxx）
	// 2. Developer Settings（仅 PIE）
	// 3. 命令行（-Experience=xxx）
	// 4. WorldSettings 默认值
	// 5. 专用服务器托管
	// 6. Fallback：B_LyraDefaultExperience
	void HandleMatchAssignmentIfNotExpectingOne();

	// =====================================================================
	// 专用服务器登录
	// =====================================================================
	bool TryDedicatedServerLogin();
	void HostDedicatedServerMatch(ECommonSessionOnlineMode OnlineMode);

	/** 专用服务器用户初始化回调 */
	UFUNCTION()
	void OnUserInitializedForDedicatedServer(const UCommonUserInfo* UserInfo, bool bSuccess, FText Error, ECommonUserPrivilege RequestedPrivilege, ECommonUserOnlineContext OnlineContext);
};
