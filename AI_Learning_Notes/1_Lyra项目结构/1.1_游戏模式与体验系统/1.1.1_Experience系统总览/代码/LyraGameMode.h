// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "ModularGameMode.h"
#include "LyraGameMode.generated.h"

class ULyraExperienceDefinition;
class ULyraPawnData;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnLyraExperienceLoaded, const ULyraExperienceDefinition* /*Experience*/);

UCLASS(Config = Game)
class ALyraGameMode : public AModularGameMode
{
	GENERATED_BODY()

public:
	ALyraGameMode(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void InitGame(const FURL& URL, UGameInstance* GameInstance) override;
	virtual UClass* GetDefaultPawnClassForController_Implementation(AController* InController) override;
	virtual APawn* SpawnDefaultPawnAtTransform_Implementation(AController* NewPlayer, const FTransform& SpawnTransform) override;
	virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;

	FOnLyraExperienceLoaded OnExperienceLoadedDelegate;

	const ULyraExperienceDefinition* GetCurrentExperience() const { return CurrentExperience; }

protected:
	void HandleMatchAssignmentIfNotExpectingOne();
	void OnExperienceLoaded(const ULyraExperienceDefinition* Experience);
	ULyraPawnData* GetPawnDataForController(AController* InController) const;

	UPROPERTY(Transient)
	TObjectPtr<const ULyraExperienceDefinition> CurrentExperience;
};
