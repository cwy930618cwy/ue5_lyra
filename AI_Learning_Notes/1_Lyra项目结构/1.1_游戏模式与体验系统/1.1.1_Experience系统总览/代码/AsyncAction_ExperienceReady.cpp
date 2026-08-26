// Copyright Epic Games, Inc. All Rights Reserved.

#include "AsyncAction_ExperienceReady.h"
#include "Engine/Engine.h"
#include "GameFramework/PlayerController.h"
#include "LyraGameState.h"
#include "LyraExperienceManagerComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AsyncAction_ExperienceReady)

UAsyncAction_ExperienceReady::UAsyncAction_ExperienceReady(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

// 静态工厂方法：创建异步操作实例
UAsyncAction_ExperienceReady* UAsyncAction_ExperienceReady::WaitForExperienceReady(UObject* WorldContextObject)
{
	UAsyncAction_ExperienceReady* Action = NewObject<UAsyncAction_ExperienceReady>();
	Action->RegisterWithGameInstance(WorldContextObject); // 注册到 GameInstance，防止被 GC
	return Action;
}

// 激活异步操作：查找 ExperienceManagerComponent 并注册回调
void UAsyncAction_ExperienceReady::Activate()
{
	Super::Activate();

	// 通过 WorldContextObject 获取 World
	UWorld* World = GEngine->GetWorldFromContextObject(GetWorldContextObject(), EGetWorldErrorMode::LogAndReturnNull);
	if (!World)
	{
		// 获取不到 World 直接结束（避免卡死）
		EndActivation();
		return;
	}

	// 获取 GameState
	ALyraGameState* GameState = World->GetGameState<ALyraGameState>();
	if (!GameState)
	{
		EndActivation();
		return;
	}

	// 获取 ExperienceManagerComponent
	ULyraExperienceManagerComponent* ExpMgrComp = GameState->GetExperienceManagerComponent();
	if (!ExpMgrComp)
	{
		EndActivation();
		return;
	}

	// 注册低优先级回调（在所有高优先级系统初始化之后再触发）
	ExpMgrComp->CallOrRegister_OnExperienceLoaded_LowPriority(
		FOnLyraExperienceLoaded::FDelegate::CreateUObject(this, &ThisClass::HandleExperienceReady));
}

// Experience 加载完成回调 → 广播 OnReady 委托并清理
void UAsyncAction_ExperienceReady::HandleExperienceReady(const ULyraExperienceDefinition* Experience)
{
	// 广播蓝图中绑定的 OnReady 委托
	OnReady.Broadcast();

	// 清理自身（标记为已完成，允许 GC 回收）
	SetReadyToDestroy();
}
