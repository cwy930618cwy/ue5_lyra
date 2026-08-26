// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintAsyncActionBase.h"
#include "AsyncAction_ExperienceReady.generated.h"

DECLARE_DYNAMIC_DELEGATE(FOnExperienceReady);

/**
 * 异步蓝图节点：等待 Experience 加载就绪
 * 
 * 使用场景：蓝图中需要在 Experience 完全加载后再执行某些逻辑
 * （如初始化 UI、播放开场动画等）。
 * 
 * 用法（蓝图中）：
 * 1. 拖入 "Wait for Experience Ready" 节点
 * 2. 连接 On Ready 输出引脚到后续逻辑
 * 3. 节点会自动监听 GameState 上的 ExperienceManagerComponent
 * 
 * 底层原理：注册到 ExperienceManagerComponent 的低优先级委托队列，
 * 如果 Experience 已经加载完毕则立即触发。
 */
UCLASS()
class LYRAGAME_API UAsyncAction_ExperienceReady : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	UAsyncAction_ExperienceReady(const FObjectInitializer& ObjectInitializer);

	/**
	 * 蓝图入口节点：等待 Experience 就绪
	 * @param WorldContextObject 世界上下文对象（任何 UObject 都行，用于获取 World）
	 * @return 异步操作实例
	 */
	UFUNCTION(BlueprintCallable, Category = "Lyra|Experience", meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject", BlueprintInternalUseOnly = "TRUE"))
	static UAsyncAction_ExperienceReady* WaitForExperienceReady(UObject* WorldContextObject);

	// 注册委托（蓝图可绑定）
	UPROPERTY(BlueprintAssignable)
	FOnExperienceReady OnReady;

	//~UBlueprintAsyncActionBase interface
	virtual void Activate() override;
	//~End of UBlueprintAsyncActionBase interface

private:
	// Experience 加载完成回调 → 广播 OnReady 并清理
	void HandleExperienceReady();
};
