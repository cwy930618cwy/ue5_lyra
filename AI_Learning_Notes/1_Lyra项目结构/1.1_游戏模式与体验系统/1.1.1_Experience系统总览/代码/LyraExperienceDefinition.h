// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Engine/DataAsset.h"
#include "LyraExperienceDefinition.generated.h"

class UGameFeatureAction;
class ULyraPawnData;
class ULyraExperienceActionSet;

UCLASS(BlueprintType, Const)
class ULyraExperienceDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	ULyraExperienceDefinition();

#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
#endif

#ifdef WITH_EDITORONLY_DATA
	virtual void UpdateAssetBundleData() override;
#endif

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Lyra|Experience")
	TArray<FString> GameFeaturesToEnable;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Lyra|Experience")
	TObjectPtr<const ULyraPawnData> DefaultPawnData;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Instanced, EditInlineNew, Category = "Lyra|Experience")
	TArray<TObjectPtr<UGameFeatureAction>> Actions;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Lyra|Experience")
	TArray<TObjectPtr<ULyraExperienceActionSet>> ActionSets;
};
