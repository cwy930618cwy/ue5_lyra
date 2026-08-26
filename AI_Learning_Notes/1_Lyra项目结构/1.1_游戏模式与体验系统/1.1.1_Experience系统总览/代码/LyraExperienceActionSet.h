// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Engine/DataAsset.h"
#include "LyraExperienceActionSet.generated.h"

class UGameFeatureAction;

/**
 * Experience ActionSet —— 可复用的 Action 组合包
 * 
 * 允许将一组常用的 Action 打包成独立的数据资产（PrimaryDataAsset），
 * 多个 Experience 可以引用同一个 ActionSet，实现配置复用。
 * 
 * 例如："基础武器包"、"多人对战动作包"、"载具系统动作包"等
 * 都可以做成独立的 ActionSet，不同 Experience 按需引用。
 * 
 * 与 ExperienceDefinition 的关系：
 * - ExperienceDefinition.Actions → 该 Experience 独有的 Action
 * - ExperienceDefinition.ActionSets → 引用的公共 Action 包（本类）
 * - 两者在加载时会合并执行
 */
UCLASS(BlueprintType, NotBlueprintable)
class ULyraExperienceActionSet : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	ULyraExperienceActionSet();

	//~UObject interface
#if WITH_EDITOR
	// 数据验证：检查所有 Action 是否有效（非空 + 递归验证）
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
#endif
	//~End of UObject interface

	//~UPrimaryDataAsset interface
#if WITH_EDITORONLY_DATA
	// 更新资产 Bundle 数据（让每个 Action 贡献自己的资源到 Bundle）
	virtual void UpdateAssetBundleData() override;
#endif
	//~End of UPrimaryDataAsset interface

public:
	// 该 ActionSet 包含的 Action 列表（加载/激活/停用/卸载时统一执行）
	UPROPERTY(EditAnywhere, Instanced, Category="Actions to Perform")
	TArray<TObjectPtr<UGameFeatureAction>> Actions;

	// 该 ActionSet 依赖的 GameFeature 插件名称列表
	UPROPERTY(EditAnywhere, Category="Feature Dependencies")
	TArray<FString> GameFeaturesToEnable;
};
