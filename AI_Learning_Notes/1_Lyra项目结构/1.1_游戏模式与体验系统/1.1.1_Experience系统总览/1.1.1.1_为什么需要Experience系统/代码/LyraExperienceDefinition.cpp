// Copyright Epic Games, Inc. All Rights Reserved.
// 
// 本文件对应的文档：01_为什么需要Experience系统.md
// 主要展示：数据验证逻辑（确保策划配置的数据合法）

#include "LyraExperienceDefinition.h"
#include "GameFeatureAction.h"       // UGameFeatureAction 基类定义

#if WITH_EDITOR
#include "Misc/DataValidation.h"     // 数据验证工具（仅编辑器）
#endif

#include UE_INLINE_GENERATED_CPP_BY_NAME(LyraExperienceDefinition)

#define LOCTEXT_NAMESPACE "LyraSystem"  // 本地化文本命名空间

/**
 * 构造函数
 * 目前为空，所有配置通过编辑器 Details 面板完成
 */
ULyraExperienceDefinition::ULyraExperienceDefinition()
{
}

#if WITH_EDITOR
/**
 * 数据验证（编辑器保存时调用）
 * 
 * 【检查内容】
 * 1. Actions 数组中是否有空指针（不允许有 null entry）
 * 2. 是否被蓝图的蓝图继承（不支持，应该用 ActionSet 组合代替）
 * 
 * 【为什么需要验证？】
 * 因为 Experience 是给策划配的，如果配错了会导致运行时崩溃，
 * 所以在编辑器保存时就检查，提前发现问题。
 */
EDataValidationResult ULyraExperienceDefinition::IsDataValid(FDataValidationContext& Context) const
{
	// 先调用父类验证，合并结果
	EDataValidationResult Result = CombineDataValidationResults(Super::IsDataValid(Context), EDataValidationResult::Valid);

	// 遍历 Actions 数组，逐个验证
	int32 EntryIndex = 0;
	for (const UGameFeatureAction* Action : Actions)
	{
		if (Action)
		{
			// Action 存在，递归验证它的数据是否合法
			EDataValidationResult ChildResult = Action->IsDataValid(Context);
			Result = CombineDataValidationResults(Result, ChildResult);
		}
		else
		{
			// Action 为空！报错提示用户删除或填充这个条目
			Result = EDataValidationResult::Invalid;
			Context.AddError(FText::Format(LOCTEXT("ActionEntryIsNull", "Null entry at index {0} in Actions"), FText::AsNumber(EntryIndex)));
		}

		++EntryIndex;
	}

	// 检查：不允许"蓝图的蓝图"继承（比如 BP_ExperienceBase → BP_MyExperience）
	// 【原因】Experience 设计为通过组合（ActionSet）扩展，而不是继承
	if (!GetClass()->IsNative())
	{
		const UClass* ParentClass = GetClass()->GetSuperClass();

		// 找到第一个原生 C++ 父类
		const UClass* FirstNativeParent = ParentClass;
		while ((FirstNativeParent != nullptr) && !FirstNativeParent->IsNative())
		{
			FirstNativeParent = FirstNativeParent->GetSuperClass();
		}

		// 如果当前父类不是原生类，说明是"蓝图的蓝图"，报错
		if (FirstNativeParent != ParentClass)
		{
			Context.AddError(FText::Format(LOCTEXT("ExperienceInheritenceIsUnsupported", "Blueprint subclasses of Blueprint experiences is not currently supported (use composition via ActionSets instead). Parent class was {0} but should be {1}."), 
				FText::AsCultureInvariant(GetPathNameSafe(ParentClass)),
				FText::AsCultureInvariant(GetPathNameSafe(FirstNativeParent))
			));
			Result = EDataValidationResult::Invalid;
		}
	}

	return Result;
}
#endif

#if WITH_EDITORONLY_DATA
/**
 * 更新资源 Bundle 数据（仅编辑器）
 * 
 * 【作用】收集所有 Action 需要的额外资源，添加到 AssetBundleData
 * 这样 AssetManager 在加载 Experience 时可以按 Bundle 分别加载
 */
void ULyraExperienceDefinition::UpdateAssetBundleData()
{
	Super::UpdateAssetBundleData();

	// 遍历所有 Action，让它们贡献自己的资源需求
	for (UGameFeatureAction* Action : Actions)
	{
		if (Action)
		{
			Action->AddAdditionalAssetBundleData(AssetBundleData);
		}
	}
}
#endif // WITH_EDITORONLY_DATA

#undef LOCTEXT_NAMESPACE
