// Copyright Epic Games, Inc. All Rights Reserved.

#include "LyraGameMode.h"
#include "LyraGameState.h"
#include "LyraExperienceManagerComponent.h"
#include "LyraExperienceDefinition.h"
#include "LyraPawnData.h"
#include "LyraLogChannels.h"

ALyraGameMode::ALyraGameMode(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void ALyraGameMode::InitGame(const FURL& URL, UGameInstance* GameInstance)
{
	Super::InitGame(URL, GameInstance);
	HandleMatchAssignmentIfNotExpectingOne();
}

void ALyraGameMode::HandleMatchAssignmentIfNotExpectingOne()
{
	if (CurrentExperience)
	{
		return;
	}

	if (ALyraGameState* GameState = GetGameState<ALyraGameState>())
	{
		if (ULyraExperienceManagerComponent* Component = GameState->FindComponentByClass<ULyraExperienceManagerComponent>())
		{
			Component->SetCurrentExperience(FPrimaryAssetId());
		}
	}
}

void ALyraGameMode::OnExperienceLoaded(const ULyraExperienceDefinition* Experience)
{
	CurrentExperience = Experience;
	OnExperienceLoadedDelegate.Broadcast(Experience);

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		APlayerController* PC = It->Get();
		if (PC && !PC->GetPawn())
		{
			SpawnDefaultPawnAtTransform(PC, PC->GetSpawnLocation());
		}
	}
}

UClass* ALyraGameMode::GetDefaultPawnClassForController_Implementation(AController* InController)
{
	if (const ULyraPawnData* PawnData = GetPawnDataForController(InController))
	{
		if (PawnData->PawnClass)
		{
			return PawnData->PawnClass;
		}
	}
	return Super::GetDefaultPawnClassForController_Implementation(InController);
}

ULyraPawnData* ALyraGameMode::GetPawnDataForController(AController* InController) const
{
	if (CurrentExperience && CurrentExperience->DefaultPawnData)
	{
		return const_cast<ULyraPawnData*>(CurrentExperience->DefaultPawnData.Get());
	}
	return nullptr;
}

APawn* ALyraGameMode::SpawnDefaultPawnAtTransform_Implementation(AController* NewPlayer, const FTransform& SpawnTransform)
{
	UClass* PawnClass = GetDefaultPawnClassForController(NewPlayer);
	if (PawnClass)
	{
		return GetWorld()->SpawnActor<APawn>(PawnClass, SpawnTransform);
	}
	return Super::SpawnDefaultPawnAtTransform_Implementation(NewPlayer, SpawnTransform);
}

void ALyraGameMode::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
	Super::HandleStartingNewPlayer_Implementation(NewPlayer);
}
