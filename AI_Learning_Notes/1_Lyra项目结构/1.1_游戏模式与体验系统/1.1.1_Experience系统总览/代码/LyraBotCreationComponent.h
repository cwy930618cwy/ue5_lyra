// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Components/GameStateComponent.h"
#include "LyraBotCreationComponent.generated.h"

class AController;
class APlayerController;
class ULyraExperienceDefinition;

/**
 * Bot 创建组件 —— 挂载在 GameState 上，负责动态创建 AI Bot
 * 
 * 核心职责：
 * 1. 监听 Experience 加载完成事件
 * 2. 根据配置在指定位置生成 AI 控制的 Pawn
 * 3. 管理 Bot 的生命周期（创建/销毁）
 * 
 * 通常在需要填充 AI 对手的游戏模式中使用（如团队竞技、大逃杀等）。
 */
UCLASS()
class LYRAGAME_API ULyraBotCreationComponent : public UGameStateComponent
{
	GENERATED_BODY()

public:

	ULyraBotCreationComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//~UActorComponent interface
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	//~End of UActorComponent interface

protected:
	// Experience 加载完成回调 → 开始创建 Bot
	void OnExperienceLoaded(const ULyraExperienceDefinition* Experience);

	// 实际创建 Bot 的逻辑（可由子类覆写或蓝图扩展）
	virtual void SpawnOneBot();

	// 移除一个 Bot
	virtual void RemoveOneBot();

protected:
	// 要生成的 Bot 控制器类
	UPROPERTY(EditDefaultsOnly, Category = "Lyra|Bot")
	TSubclassOf<AController> BotControllerClass;

	// 要生成的 Bot Pawn 类（为空则使用 Experience 的默认 PawnData）
	UPROPERTY(EditDefaultsOnly, Category = "Lyra|Bot")
	TSubclassOf<APawn> BotPawnClass;

	// 要创建的 Bot 数量
	UPROPERTY(EditDefaultsOnly, Category = "Lyra|Bot")
	int32 NumBotsToCreate = 0;

	// 已创建的 Bot 列表
	UPROPERTY(Transient)
	TArray<TObjectPtr<AController>> CreatedBots;
};
