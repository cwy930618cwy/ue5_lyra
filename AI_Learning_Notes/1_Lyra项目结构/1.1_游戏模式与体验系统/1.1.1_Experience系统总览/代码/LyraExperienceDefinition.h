// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Engine/DataAsset.h"
#include "LyraExperienceDefinition.generated.h"

// 前向声明：告诉编译器这些类存在，但不需要完整定义（避免循环引用）
class UGameFeatureAction;        // GameFeature 动作基类
class ULyraPawnData;             // Pawn 数据资产（定义角色配置）
class ULyraExperienceActionSet;  // Action 集合（可复用的 Action 组合包）

/**
 * 体验定义（Experience Definition）
 * 
 * 一份"游戏模式配置文件"，策划在编辑器里创建实例、填好参数，就定义了一种玩法。
 * 
 * 继承链：UObject → UDataAsset → UPrimaryDataAsset → ULyraExperienceDefinition
 * UPrimaryDataAsset = 可以在编辑器中右键创建的独立资产（有唯一 ID）
 */
UCLASS(BlueprintType, Const)  // BlueprintType=蓝图可见, Const=运行时不可修改
class ULyraExperienceDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	ULyraExperienceDefinition();

	// 数据验证：保存时检查配置是否合法（仅编辑器）
#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
#endif

	// 资源收集：告诉引擎这个 Experience 需要哪些资源（仅编辑器）
#ifdef WITH_EDITORONLY_DATA
	virtual void UpdateAssetBundleData() override;
#endif

public:
	// ① 要启用的 GameFeature 插件列表（如 ["ShooterCore", "Teams"]）
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Lyra|Experience")
	TArray<FString> GameFeaturesToEnable;

	// ② 默认 Pawn 数据（决定玩家用什么角色）
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Lyra|Experience")
	TObjectPtr<const ULyraPawnData> DefaultPawnData;

	// ③ 内联 Action 列表（Instanced=每个实例独立, EditInlineNew=直接在详情面板添加子对象）
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Instanced, EditInlineNew, Category = "Lyra|Experience")
	TArray<TObjectPtr<UGameFeatureAction>> Actions;

	// ④ 引用的外部 ActionSet（可复用的 Action 组合包，多个 Experience 共享）
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Lyra|Experience")
	TArray<TObjectPtr<ULyraExperienceActionSet>> ActionSets;
};
