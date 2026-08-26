// Copyright Epic Games, Inc. All Rights Reserved.
// 
// 本文件对应的文档：01_为什么需要Experience系统.md
// 核心概念：把"玩法差异"从代码中抽出来，放到数据资产里

#pragma once

#include "Engine/DataAsset.h"
#include "LyraExperienceDefinition.generated.h"

// 前向声明：告诉编译器这些类存在，但不需要完整定义（避免循环引用）
class UGameFeatureAction;        // GameFeature 动作基类
class ULyraPawnData;             // Pawn 数据资产（定义角色配置）
class ULyraExperienceActionSet;  // Action 集合（可复用的 Action 组合）

/**
 * ============================================
 * 体验定义（Experience Definition）
 * ============================================
 * 
 * 【一句话概括】
 * 一份"游戏模式配置文件"，策划在编辑器里创建实例、填好参数，就定义了一种玩法。
 * 
 * 【为什么需要它？】
 * 传统做法：团队竞技和个人竞技要写两个 GameMode 子类，代码大量重复。
 * Lyra 做法：同一套 C++ 代码 + 不同的 ExperienceDefinition 配置 = 不同游戏模式。
 * 
 * 【三个关键字段对应文档中的表格】
 * - GameFeaturesToEnable: 要启用的插件列表（如 ["ShooterCore", "Teams"]）
 * - DefaultPawnData: 玩家用什么角色（标准士兵 vs 快速移动兵）
 * - Actions/ActionSets: 要执行的逻辑（队伍UI vs 个人计分UI）
 * 
 * 【继承关系】
 * UObject → UDataAsset → UPrimaryDataAsset → ULyraExperienceDefinition
 */
UCLASS(BlueprintType, Const)  // BlueprintType=蓝图可见, Const=运行时不可修改
class ULyraExperienceDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	ULyraExperienceDefinition();

	//~UObject interface
#if WITH_EDITOR
	// 编辑器保存时验证数据是否合法
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
#endif
	//~End of UObject interface

	//~UPrimaryDataAsset interface
#if WITH_EDITORONLY_DATA
	// 更新资源 Bundle 数据（用于按平台/角色分别加载不同资源）
	virtual void UpdateAssetBundleData() override;
#endif
	//~End of UPrimaryDataAsset interface

public:
	// =====================================================================
	// ① 要启用的 GameFeature 插件列表
	// =====================================================================
	// 【作用】决定这局游戏启用哪些功能模块
	// 【示例】团队竞技: ["ShooterCore", "Teams"]  个人竞技: ["ShooterCore"]
	// 【原理】ExperienceManagerComponent 会根据这个列表加载对应插件
	UPROPERTY(EditDefaultsOnly, Category = Gameplay)
	TArray<FString> GameFeaturesToEnable;

	// =====================================================================
	// ② 默认 Pawn 数据（玩家用什么角色）
	// =====================================================================
	// 【作用】决定玩家生成什么类型的角色
	// 【示例】团队竞技用"标准士兵"，个人竞技用"快速移动兵"
	// 【注意】const 指针表示 Experience 只读取不修改 PawnData
	UPROPERTY(EditDefaultsOnly, Category=Gameplay)
	TObjectPtr<const ULyraPawnData> DefaultPawnData;

	// =====================================================================
	// ③ 直接定义的 Action 列表（内联，专属本 Experience）
	// =====================================================================
	// 【作用】定义加载时要执行的动作（如初始化 UI、设置计分规则）
	// 【Instanced】表示这些 Action 是本 Experience 的内联子对象
	// 【生命周期】每个 Action 会收到三个回调：
	//   OnGameFeatureRegistering() → OnGameFeatureLoading() → OnGameFeatureActivating()
	UPROPERTY(EditDefaultsOnly, Instanced, Category="Actions")
	TArray<TObjectPtr<UGameFeatureAction>> Actions;

	// =====================================================================
	// ④ 引用的 ActionSet（复用其他定义好的 Action 组合）
	// =====================================================================
	// 【与 Actions 的区别】Actions 是内联的（专属），ActionSets 是外部引用（可复用）
	// 【典型场景】多个 Experience 共享同一组"基础 UI Action"
	// 【加载时】会合并：Experience 自己的 Actions + 每个 ActionSet 的 Actions
	UPROPERTY(EditDefaultsOnly, Category=Gameplay)
	TArray<TObjectPtr<ULyraExperienceActionSet>> ActionSets;
};
