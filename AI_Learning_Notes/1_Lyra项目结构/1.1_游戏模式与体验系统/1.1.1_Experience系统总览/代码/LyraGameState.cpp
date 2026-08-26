// Copyright Epic Games, Inc. All Rights Reserved.

#include "LyraGameState.h"
#include "LyraExperienceManagerComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(LyraGameState)

ALyraGameState::ALyraGameState(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

// PreInitializeComponents：在组件初始化前创建 ExperienceManagerComponent
void ALyraGameState::PreInitializeComponents()
{
	Super::PreInitializeComponents();

	// 创建 ExperienceManagerComponent 并设为复制（网络同步）
	ExperienceManagerComponent = NewObject<ULyraExperienceManagerComponent>(this);
	ExperienceManagerComponent->SetIsReplicated(true);
}

// PostInitializeComponents：组件初始化后的收尾工作
void ALyraGameState::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

// 便捷查找方法
ULyraExperienceManagerComponent* ALyraGameState::GetExperienceManagerComponent() const
{
	return ExperienceManagerComponent;
}
