// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "ModularGameState.h"
#include "LyraGameState.generated.h"

class ULyraExperienceManagerComponent;

/**
 * Lyra GameState —— 承载 ExperienceManagerComponent
 * 
 * GameState 是每个关卡全局唯一的 Actor，所有客户端都能访问。
 * Lyra 把 ExperienceManagerComponent 放在 GameState 上，
 * 这样 Experience 的加载状态对所有玩家都是统一的。
 * 
 * 继承自 AModularGameState（支持组件化扩展）。
 */
UCLASS(Config = Game)
class ALyraGameState : public AModularGameState
{
	GENERATED_BODY()

public:
	ALyraGameState(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//~AActor interface
	virtual void PreInitializeComponents() override;
	virtual void PostInitializeComponents() override;
	//~End of AActor interface

	// 便捷方法：查找并返回 ExperienceManagerComponent
	ULyraExperienceManagerComponent* GetExperienceManagerComponent() const;

protected:
	// Experience 管理器组件（在 PreInitializeComponents 中创建）
	UPROPERTY()
	TObjectPtr<ULyraExperienceManagerComponent> ExperienceManagerComponent;
};
