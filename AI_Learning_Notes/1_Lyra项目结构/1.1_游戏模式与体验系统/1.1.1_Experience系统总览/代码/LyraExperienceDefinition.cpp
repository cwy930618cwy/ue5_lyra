// Copyright Epic Games, Inc. All Rights Reserved.

#include "LyraExperienceDefinition.h"
#include "LyraExperienceActionSet.h"
#include "GameFeatureAction.h"
#include "LyraLogChannels.h"
#include "Misc/DataValidation.h"

#define LOCTEXT_NAMESPACE "LyraExperienceDefinition"

ULyraExperienceDefinition::ULyraExperienceDefinition()
{
	// 空实现，所有配置通过编辑器 Details 面板完成
}

#if WITH_EDITOR

// 数据验证：保存资产时自动调用，确保配置不出错
EDataValidationResult ULyraExperienceDefinition::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = CombineDataValidationResults(Super::IsDataValid(Context), EDataValidationResult::Valid);

	int32 ActionIndex = 0;
	for (const TObjectPtr<UGameFeatureAction>& Action : Actions)
	{
		// 检查 1：Action 不能为空指针
		if (Action == nullptr)
		{
			Context.AddError(FText::Format(LOCTEXT("NullAction", "Actions[{0}] is null"), FText::AsNumber(ActionIndex)));
			Result = EDataValidationResult::Invalid;
		}
		else
		{
			// 检查 2：递归验证每个 Action 内部的子对象
			Result = CombineDataValidationResults(Result, Action->IsDataValid(Context));
		}

		// 检查 3：禁止"蓝图的蓝图"继承——应用 ActionSet 组合而非蓝图继承链
		// 原因：如果允许蓝图继承蓝图，会导致深层继承链，违背"组合优于继承"的设计原则
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

// 资源收集：告诉引擎这个 Experience 加载后还需要哪些额外资源
void ULyraExperienceDefinition::UpdateAssetBundleData()
{
	Super::UpdateAssetBundleData();

	// 遍历所有 Action，让每个 Action 把自己需要的资源注册进来
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
