// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "ModularGameMode.h"
#include "LyraGameMode.generated.h"

class ULyraExperienceDefinition;
class ULyraPawnData;

// 委托声明：Experience 加载完成时广播（其他系统可以监听这个事件）
DECLARE_MULTICAST_DELEGATE_OneParam(FOnLyraExperienceLoaded, const ULyraExperienceDefinition* /*Experience*/);

/**
 * ALyraGameMode — 读取 Experience 配置的 GameMode
 * 
 * 核心职责：
 * 1. 游戏启动时确定使用哪个 Experience
 * 2. Experience 加载完成后为在场玩家生成 Pawn
 * 3. 动态决定每个玩家的 Pawn 类型（由 Experience 配置决定）
 * 
 * 继承链：AModularGameMode → ALyraGameMode
 * AModularGameMode = 支持组件化扩展的 GameMode 基类
 */
UCLASS(Config = Game)
class ALyraGameMode : public AModularGameMode
{
	GENERATED_BODY()

public:
	ALyraGameMode(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	// 游戏初始化入口（比 BeginPlay 更早调用），触发 Experience 加载
	virtual void InitGame(const FURL& URL, UGameInstance* GameInstance) override;

	// 返回 Controller 对应的 Pawn 类型（由 Experience 配置决定）
	virtual UClass* GetDefaultPawnClassForController_Implementation(AController* InController) override;

	// 在指定位置生成 Pawn（用 Experience 配置的 Pawn 类型）
	virtual APawn* SpawnDefaultPawnAtTransform_Implementation(AController* NewPlayer, const FTransform& SpawnTransform) override;

	// 新玩家加入时的处理
	virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;

	// Experience 加载完成的委托（其他系统如 BotCreationComponent 可以监听）
	FOnLyraExperienceLoaded OnExperienceLoadedDelegate;

	// 获取当前加载的 Experience
	const ULyraExperienceDefinition* GetCurrentExperience() const { return CurrentExperience; }

protected:
	// 按优先级确定用哪个 Experience（WorldSettings > 命令行 > 项目设置）
	void HandleMatchAssignmentIfNotExpectingOne();

	// Experience 加载完成回调：广播委托 + 为在场玩家生成 Pawn
	void OnExperienceLoaded(const ULyraExperienceDefinition* Experience);

	// 从 Experience 配置中查找 Controller 对应的 PawnData
	ULyraPawnData* GetPawnDataForController(AController* InController) const;

	// 当前加载的 Experience（Transient=不序列化，每次运行时重新加载）
	UPROPERTY(Transient)
	TObjectPtr<const ULyraExperienceDefinition> CurrentExperience;
};
