// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Components/GameStateComponent.h"
#include "LoadingProcessInterface.h"

#include "LyraExperienceManagerComponent.generated.h"

namespace UE::GameFeatures { struct FResult; }

class ULyraExperienceDefinition;

// Experience 加载完成时广播的委托（参数为加载好的 Experience 定义）
DECLARE_MULTICAST_DELEGATE_OneParam(FOnLyraExperienceLoaded, const ULyraExperienceDefinition* /*Experience*/);

// Experience 加载状态机枚举 —— 7 个阶段，从 Unloaded 到 Loaded
enum class ELyraExperienceLoadState
{
	Unloaded,                 // 未加载（初始状态）
	Loading,                  // 正在异步加载资源
	LoadingGameFeatures,      // 正在加载 GameFeature 插件
	LoadingChaosTestingDelay, // Chaos Testing 随机延迟（测试用）
	ExecutingActions,         // 正在执行所有 Action
	Loaded,                   // 完全加载完毕
	Deactivating              // 正在卸载
};

/**
 * Experience 管理器组件 —— 挂载在 GameState 上
 * 
 * 核心职责：
 * 1. 接收 Experience ID → 异步加载 Experience 资源
 * 2. 加载完成后启用 GameFeature 插件
 * 3. 插件就绪后执行所有 Action（注册→加载→激活）
 * 4. 全部完成后广播委托，通知其他系统
 * 
 * 网络同步：CurrentExperience 通过 ReplicatedUsing=OnRep 从服务器同步到客户端，
 * 客户端收到后自动开始本地加载流程。
 */
UCLASS()
class LYRAGAME_API ULyraExperienceManagerComponent final : public UGameStateComponent, public ILoadingProcessInterface
{
	GENERATED_BODY()

public:

	ULyraExperienceManagerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//~UActorComponent interface
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	//~End of UActorComponent interface

	//~ILoadingProcessInterface interface
	// 判断是否需要显示加载界面（Experience 未加载完就返回 true）
	virtual bool ShouldShowLoadingScreen(FString& OutReason) const override;
	//~End of ILoadingProcessInterface

	// 设置当前要加载的 Experience（传入主资产 ID，只能调用一次）
	void SetCurrentExperience(FPrimaryAssetId ExperienceId);

	// 注册高优先级回调：Experience 加载完成后最先调用（用于子系统初始化等）
	void CallOrRegister_OnExperienceLoaded_HighPriority(FOnLyraExperienceLoaded::FDelegate&& Delegate);

	// 注册普通优先级回调
	void CallOrRegister_OnExperienceLoaded(FOnLyraExperienceLoaded::FDelegate&& Delegate);

	// 注册低优先级回调：最后调用
	void CallOrRegister_OnExperienceLoaded_LowPriority(FOnLyraExperienceLoaded::FDelegate&& Delegate);

	// 获取当前 Experience（必须已完全加载，否则 check 断言失败）
	const ULyraExperienceDefinition* GetCurrentExperienceChecked() const;

	// 判断 Experience 是否已完全加载
	bool IsExperienceLoaded() const;

private:
	// 网络复制回调：客户端收到 CurrentExperience 后触发加载
	UFUNCTION()
	void OnRep_CurrentExperience();

	// === 加载流程四步走 ===
	void StartExperienceLoad();                    // Step1: 开始异步加载 Experience 资源
	void OnExperienceLoadComplete();               // Step2: 资源加载完 → 开始加载 GameFeature 插件
	void OnGameFeaturePluginLoadComplete(const UE::GameFeatures::FResult& Result); // Step3: 每个插件完成计数-1
	void OnExperienceFullLoadCompleted();          // Step4: 全部就绪 → 执行 Actions + 广播

	// === 卸载流程 ===
	void OnActionDeactivationCompleted();   // 单个 Action 卸载完成回调（计数器同步）
	void OnAllActionsDeactivated();         // 所有 Action 卸载完成 → 回到 Unloaded

private:
	// 当前 Experience 定义（网络复制，客户端通过 OnRep_CurrentExperience 触发加载）
	UPROPERTY(ReplicatedUsing=OnRep_CurrentExperience)
	TObjectPtr<const ULyraExperienceDefinition> CurrentExperience;

	// 当前加载状态
	ELyraExperienceLoadState LoadState = ELyraExperienceLoadState::Unloaded;

	// GameFeature 插件加载计数器（每加载完一个 -1，归零时进入下一步）
	int32 NumGameFeaturePluginsLoading = 0;
	// 需要加载的 GameFeature 插件 URL 列表
	TArray<FString> GameFeaturePluginURLs;

	// 卸载时的异步观察者计数（等待所有异步 Action 卸载完成）
	int32 NumObservedPausers = 0;
	int32 NumExpectedPausers = 0;

	/** 三级优先级委托队列：高 → 中 → 低，加载完成后依次广播并清空 */
	FOnLyraExperienceLoaded OnExperienceLoaded_HighPriority;
	FOnLyraExperienceLoaded OnExperienceLoaded;
	FOnLyraExperienceLoaded OnExperienceLoaded_LowPriority;
};
