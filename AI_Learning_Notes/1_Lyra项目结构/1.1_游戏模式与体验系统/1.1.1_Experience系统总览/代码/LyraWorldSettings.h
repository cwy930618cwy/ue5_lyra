// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameFramework/WorldSettings.h"
#include "LyraWorldSettings.generated.h"

class ULyraExperienceDefinition;

/**
 * Lyra WorldSettings —— 关卡级默认 Experience 配置
 * 
 * 每个关卡的 WorldSettings 里可以指定一个默认 Experience，
 * 当 GameMode 没有显式指定 Experience 时，就用这个作为 fallback。
 * 
 * 优先级：GameMode 显式指定 > WorldSettings 默认值 > 报错
 */
UCLASS(Config = Game)
class ALyraWorldSettings : public AWorldSettings
{
	GENERATED_BODY()

public:
	ALyraWorldSettings(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	// 获取该关卡默认的 Experience ID（从 DefaultExperience 资产读取）
	FPrimaryAssetId GetDefaultExperienceId() const;

protected:
	//~AActor interface
	virtual void PostRegisterAllComponents() override;
	//~End of AActor interface

#if WITH_EDITOR
	// 编辑器属性变更回调（用于验证 Experience 数据有效性）
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

protected:
	// 该关卡默认使用的 Experience（在编辑器 Details 面板中配置）
	UPROPERTY(EditDefaultsOnly, Category = "Lyra|World")
	TSoftObjectPtr<const ULyraExperienceDefinition> DefaultExperience;
};
