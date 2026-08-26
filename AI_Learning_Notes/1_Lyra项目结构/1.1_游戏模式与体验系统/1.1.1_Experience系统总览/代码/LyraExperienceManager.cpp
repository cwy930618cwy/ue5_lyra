// Copyright Epic Games, Inc. All Rights Reserved.

#include "LyraExperienceManager.h"
#include "Engine/Engine.h"
#include "Subsystems/SubsystemCollection.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(LyraExperienceManager)

#if WITH_EDITOR

// PIE 开始时重置所有请求计数
void ULyraExperienceManager::OnPlayInEditorBegun()
{
	ensure(GameFeaturePluginRequestCountMap.IsEmpty());
	GameFeaturePluginRequestCountMap.Empty();
}

// 通知有插件被请求激活：找到该插件的计数条目并 +1
void ULyraExperienceManager::NotifyOfPluginActivation(const FString PluginURL)
{
	if (GIsEditor)
	{
		ULyraExperienceManager* ExperienceManagerSubsystem = GEngine->GetEngineSubsystem<ULyraExperienceManager>();
		check(ExperienceManagerSubsystem);

		// FindOrAdd：不存在则创建条目（值为0），然后递增
		int32& Count = ExperienceManagerSubsystem->GameFeaturePluginRequestCountMap.FindOrAdd(PluginURL);
		++Count;
	}
}

// 请求停用插件：计数-1，只有最后一个使用者返回 true
bool ULyraExperienceManager::RequestToDeactivatePlugin(const FString PluginURL)
{
	if (GIsEditor)
	{
		ULyraExperienceManager* ExperienceManagerSubsystem = GEngine->GetEngineSubsystem<ULyraExperienceManager>();
		check(ExperienceManagerSubsystem);

		// FindChecked：必须存在（因为之前一定 Notify 过）
		int32& Count = ExperienceManagerSubsystem->GameFeaturePluginRequestCountMap.FindChecked(PluginURL);
		--Count;

		// 计数归零 → 没有任何会话再需要这个插件了，可以安全停用
		if (Count == 0)
		{
			ExperienceManagerSubsystem->GameFeaturePluginRequestCountMap.Remove(PluginURL);
			return true; // 返回 true = 应该执行停用
		}

		return false; // 还有其他会话在使用，不能停用
	}

	return true; // 非编辑器环境直接允许停用
}

#endif
