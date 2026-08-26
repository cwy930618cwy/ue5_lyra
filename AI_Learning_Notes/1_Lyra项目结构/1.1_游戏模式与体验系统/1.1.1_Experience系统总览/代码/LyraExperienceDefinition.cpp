// Copyright Epic Games, Inc. All Rights Reserved.

#include "LyraExperienceDefinition.h"
#include "LyraExperienceActionSet.h"
#include "GameFeatureAction.h"
#include "LyraLogChannels.h"
#include "Misc/DataValidation.h"

#define LOCTEXT_NAMESPACE "LyraExperienceDefinition"

ULyraExperienceDefinition::ULyraExperienceDefinition()
{
}

#if WITH_EDITOR

EDataValidationResult ULyraExperienceDefinition::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = CombineDataValidationResults(Super::IsDataValid(Context), EDataValidationResult::Valid);

	int32 ActionIndex = 0;
	for (const TObjectPtr<UGameFeatureAction>& Action : Actions)
	{
		if (Action == nullptr)
		{
			Context.AddError(FText::Format(LOCTEXT("NullAction", "Actions[{0}] is null"), FText::AsNumber(ActionIndex)));
			Result = EDataValidationResult::Invalid;
		}
		else
		{
			Result = CombineDataValidationResults(Result, Action->IsDataValid(Context));
		}

		if (!Action->GetClass()->IsNative())
		{
			const UClass* SuperClass = Action->GetClass()->GetSuperClass();
			if ((SuperClass != nullptr) && !SuperClass->IsNative())
			{
				Context.AddError(FText::Format(
					LOCTEXT("NonNativeParent", "Actions[{0}] has a non-native parent class '{1}'. Use ActionSet composition instead of BP inheritance."),
					FText::AsNumber(ActionIndex),
					SuperClass->GetDisplayName()));
				Result = EDataValidationResult::Invalid;
			}
		}

		ActionIndex++;
	}

	return Result;
}

#endif // WITH_EDITOR

#ifdef WITH_EDITORONLY_DATA

void ULyraExperienceDefinition::UpdateAssetBundleData()
{
	Super::UpdateAssetBundleData();

	for (const TObjectPtr<UGameFeatureAction>& Action : Actions)
	{
		if (Action != nullptr)
		{
			Action->AddGameFeatureResources(*this);
		}
	}
}

#endif // WITH_EDITORONLY_DATA

#undef LOCTEXT_NAMESPACE
