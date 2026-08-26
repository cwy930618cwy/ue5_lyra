// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Engine/DataAsset.h"
#include "LyraUserFacingExperienceDefinition.generated.h"

class ULyraExperienceDefinition;
class ULyraPawnData;
class AActor;

/**
 * 面向用户的 Experience 定义 —— 主菜单玩法列表项
 * 
 * 与 ULyraExperienceDefinition（内部用）不同，本类是**给玩家看的**：
 * - 包含显示名称、描述、缩略图等 UI 信息
 * - 用于主菜单的"开始游戏"列表、大厅选择界面等
 * - 内部引用真正的 ExperienceDefinition 来执行加载
 * 
 * 设计目的：把"策划配置的游戏模式列表"和"程序用的 Experience 加载"分离开。
 * 策划可以在不碰代码的情况下增删改主菜单里的玩法选项。
 */
UCLASS(BlueprintType, NotBlueprintable)
class ULyraUserFacingExperienceDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	ULyraUserFacingExperienceDefinition();

	//~UPrimaryDataAsset interface
#if WITH_EDITORONLY_DATA
	// 更新资产 Bundle 数据
	virtual void UpdateAssetBundleData() override;
#endif
	//~End of UPrimaryDataAsset interface

public:
	// 该玩法对应的真正 Experience 定义（实际加载时用的）
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Lyra|Experience")
	TSoftObjectPtr<const ULyraExperienceDefinition> Experience;

	// 在主菜单上显示的友好名称（本地化）
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Lyra|Experience")
	FText UserFacingName;

	// 在主菜单上显示的描述文本（本地化）
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Lyra|Experience")
	FText UserFacingDescription;

	// 主菜单上的缩略图/图标
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Lyra|Experience")
	TSoftObjectPtr<UTexture2D> Image;

	// 是否显示给玩家（false = 隐藏，用于调试或尚未开放的玩法）
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Lyra|Experience")
	bool bVisible = true;

	// 排序权重（数字越小越靠前）
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Lyra|Experience")
	int32 SortOrder = 0;

	// 该玩法需要的最少玩家数
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Lyra|Experience")
	int32 MinPlayerCount = 1;

	// 该玩法允许的最大玩家数
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Lyra|Experience")
	int32 MaxPlayerCount = 8;

	// 地图/关卡名称（用于显示）
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Lyra|Experience")
	FText MapDisplayName;

	// 游戏模式简称（如 "FFA"、"CTF"、"Crazy" 等，用于 UI 显示）
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Lyra|Experience")
	FString GameModeShortName;

	// 创建该玩法所需的 PawnData（覆盖 Experience 中的默认值）
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Lyra|Experience")
	TSoftObjectPtr<const ULyraPawnData> PawnData;

	// 每侧队伍的玩家数量（用于队伍制游戏模式）
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Lyra|Experience", meta = (ClampMin = 0))
	int32 NumPlayersPerTeam = 1;

	// 该玩法是否支持单人游玩
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Lyra|Experience")
	bool bSupportsSinglePlayer = true;

	// === 以下方法由蓝图/UI 层调用 ===

	// 获取当前平台的最大玩家数（考虑平台限制后的值）
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Lyra|Experience")
	int32 GetMaxPlayerCountForCurrentPlatform() const;

	// 判断该平台是否支持此玩法
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Lyra|Experience")
	bool IsSupportedForCurrentPlatform() const;

	// 获取该玩法支持的输入设备类型（手柄/键鼠/触屏）
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Lyra|Experience")
	EGamepadInputType GetGamepadInputType() const;

	// 获取该玩法的在线模式（单机/联机/仅主机）
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Lyra|Experience")
	EOnlineMode GetOnlineMode() const;

	// 获取该玩法的地图名（用于显示）
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Lyra|Experience")
	FString GetMapName() const;

	// 获取该玩法的地图展示名（用于 UI 显示）
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Lyra|Experience")
	FText GetMapDisplayName() const;

	// 获取该玩法的缩略图
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Lyra|Experience")
	UTexture2D* GetImage() const;

	// 获取该玩法的游戏模式简称
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Lyra|Experience")
	FString GetGameModeShortName() const;

	// 获取该玩法的 PawnData
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Lyra|Experience")
	const ULyraPawnData* GetPawnData() const;

	// 获取该玩法的实际 Experience 定义
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Lyra|Experience")
	const ULyraExperienceDefinition* GetExperience() const;

	// 获取该玩法的显示名称
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Lyra|Experience")
	FText GetUserFacingName() const;

	// 获取该玩法的描述文本
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Lyra|Experience")
	FText GetUserFacingDescription() const;

	// 获取每侧队伍的玩家数量
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Lyra|Experience")
	int32 GetNumPlayersPerTeam() const;

	// 获取该玩法支持的队伍数量
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Lyra|Experience")
	int32 GetNumTeams() const;

	// 获取该玩法是否支持单人游玩
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Lyra|Experience")
	bool GetSupportsSinglePlayer() const;

	// 获取该玩法的排序权重
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Lyra|Experience")
	int32 GetSortOrder() const;

	// 判断该玩法是否对玩家可见
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Lyra|Experience")
	bool IsVisible() const;

	// 获取该玩法的最小玩家数
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Lyra|Experience")
	int32 GetMinPlayerCount() const;
};
