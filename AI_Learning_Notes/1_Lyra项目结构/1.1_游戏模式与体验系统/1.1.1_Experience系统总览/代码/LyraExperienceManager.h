// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Subsystems/EngineSubsystem.h"
#include "LyraExperienceManager.generated.h"

/**
 * Experience 管理器 —— EngineSubsystem
 * 
 * 核心职责：PIE 多会话场景下的 GameFeature 插件激活仲裁
 * 
 * 问题背景：PIE 时可能同时运行多个游戏窗口（多个 World），
 * 它们可能都需要同一个 GameFeature 插件。如果第一个窗口关闭时
 * 就直接停用插件，其他窗口会受影响。
 * 
 * 解决方案：引用计数 —— 第一个请求激活，最后一个请求才停用。
 * 
 * 注意：仅在 WITH_EDITOR（PIE）模式下生效，打包后直接返回 true。
 */
UCLASS(MinimalAPI)
class ULyraExperienceManager : public UEngineSubsystem
{
	GENERATED_BODY()

public:
#if WITH_EDITOR
	// PIE 开始时重置所有请求计数（确保每次 Play 都是干净状态）
	LYRAGAME_API void OnPlayInEditorBegun();

	// 通知有插件被请求激活（计数+1）
	static void NotifyOfPluginActivation(const FString PluginURL);
	// 请求停用插件（计数-1，只有计数归零才返回 true 表示可以真正停用）
	static bool RequestToDeactivatePlugin(const FString PluginURL);
#else
	// 非编辑器环境（打包后）：不需要仲裁，直接允许
	static void NotifyOfPluginActivation(const FString PluginURL) {}
	static bool RequestToDeactivatePlugin(const FString PluginURL) { return true; }
#endif

private:
	// GameFeature 插件 URL → 激活请求计数的映射表
	TMap<FString, int32> GameFeaturePluginRequestCountMap;
};
