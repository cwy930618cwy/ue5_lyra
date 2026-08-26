// Copyright Epic Games, Inc. All Rights Reserved.

#include "LyraUserFacingExperienceDefinition.h"
#include "LyraExperienceDefinition.h"
#include "LyraPawnData.h"
#include "LyraGameplayTags.h"
#include "System/LyraAssetManager.h"
#include "System/LyraSignificanceManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(LyraUserFacingExperienceDefinition)

ULyraUserFacingExperienceDefinition::ULyraUserFacingExperienceDefinition()
{
}

#if WITH_EDITORONLY_DATA
// 更新 Bundle 数据
void ULyraUserFacingExperienceDefinition::UpdateAssetBundleData()
{
	Super::UpdateAssetBundleData();

	// 把引用的 Experience 和 PawnData 加入 Bundle（确保它们被一起加载）
	if (Experience.IsValid())
	{
		AddBundleDataAsset(FLyraBundles::Equipped, Experience.ToSoftObjectPath());
	}
	if (PawnData.IsValid())
	{
		AddBundleDataAsset(FLyraBundles::Equipped, PawnData.ToSoftObjectPath());
	}
}
#endif

// 获取当前平台的最大玩家数（考虑平台限制）
int32 ULyraUserFacingExperienceDefinition::GetMaxPlayerCountForCurrentPlatform() const
{
	// 基础最大值 = 配置的 MaxPlayerCount
	int32 Result = MaxPlayerCount;

	// TODO: 根据当前平台能力进一步限制（如移动端可能减少人数）
	
	return Result;
}

// 判断当前平台是否支持此玩法
bool ULyraUserFacingExperienceDefinition::IsSupportedForCurrentPlatform() const
{
	// 目前所有平台都支持（未来可根据平台特性限制）
	return true;
}

// 获取该玩法支持的输入设备类型
EGamepadInputType ULyraUserFacingExperienceDefinition::GetGamepadInputType() const
{
	// 默认支持标准手柄输入
	return EGamepadInputType::Standard;
}

// 获取该玩法的在线模式
EOnlineMode ULyraUserFacingExperienceDefinition::GetOnlineMode() const
{
	// 根据玩家数量判断：1人 = 单机，多人 = 联机
	if (MaxPlayerCount <= 1)
	{
		return EOnlineMode::SinglePlayer;
	}
	else
	{
		return EOnlineMode::OnlineMultiplayer;
	}
}

// 获取地图名（用于显示）
FString ULyraUserFacingExperienceDefinition::GetMapName() const
{
	// 从 Experience 关联的地图资产获取名称
	if (Experience.IsValid())
	{
		return Experience.ToSoftObjectPath().GetAssetName();
	}
	return FString();
}

// 获取地图展示名
FText ULyraUserFacingExperienceDefinition::GetMapDisplayName() const
{
	return MapDisplayName;
}

// 获取缩略图
UTexture2D* ULyraUserFacingExperienceDefinition::GetImage() const
{
	return Image.LoadSynchronous();
}

// 获取游戏模式简称
FString ULyraUserFacingExperienceDefinition::GetGameModeShortName() const
{
	return GameModeShortName;
}

// 获取 PawnData
const ULyraPawnData* ULyraUserFacingExperienceDefinition::GetPawnData() const
{
	return PawnData.LoadSynchronous();
}

// 获取实际的 Experience 定义
const ULyraExperienceDefinition* ULyraUserFacingExperienceDefinition::GetExperience() const
{
	return Experience.LoadSynchronous();
}

// 获取显示名称
FText ULyraUserFacingExperienceDefinition::GetUserFacingName() const
{
	return UserFacingName;
}

// 获取描述文本
FText ULyraUserFacingExperienceDefinition::GetUserFacingDescription() const
{
	return UserFacingDescription;
}

// 获取每侧队伍玩家数
int32 ULyraUserFacingExperienceDefinition::GetNumPlayersPerTeam() const
{
	return NumPlayersPerTeam;
}

// 获取队伍数量
int32 ULyraUserFacingExperienceDefinition::GetNumTeams() const
{
	// 根据最大玩家数和每队人数计算队伍数
	if (NumPlayersPerTeam > 0)
	{
		return MaxPlayerCount / NumPlayersPerTeam;
	}
	return 1;
}

// 是否支持单人游玩
bool ULyraUserFacingExperienceDefinition::GetSupportsSinglePlayer() const
{
	return bSupportsSinglePlayer;
}

// 获取排序权重
int32 ULyraUserFacingExperienceDefinition::GetSortOrder() const
{
	return SortOrder;
}

// 是否对玩家可见
bool ULyraUserFacingExperienceDefinition::IsVisible() const
{
	return bVisible;
}

// 获取最小玩家数
int32 ULyraUserFacingExperienceDefinition::GetMinPlayerCount() const
{
	return MinPlayerCount;
}
