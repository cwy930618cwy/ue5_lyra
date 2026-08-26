// Copyright Epic Games, Inc. All Rights Reserved.

#include "LyraWorldSettings.h"
#include "LyraExperienceDefinition.h"
#include "LyraLogChannels.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(LyraWorldSettings)

ALyraWorldSettings::ALyraWorldSettings(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

// 获取默认 Experience 的主资产 ID
FPrimaryAssetId ALyraWorldSettings::GetDefaultExperienceId() const
{
	return DefaultExperience.ToSoftObjectPath().GetAssetId();
}

// 组件注册完成后调用
void ALyraWorldSettings::PostRegisterAllComponents()
{
	Super::PostRegisterAllComponents();
}

#if WITH_EDITOR
// 编辑器中修改属性时的验证
void ALyraWorldSettings::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	// 如果修改的是 DefaultExperience，验证其数据有效性
	if (PropertyChangedEvent.Property && 
		(PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(ALyraWorldSettings, DefaultExperience)))
	{
		if (DefaultExperience.IsValid())
		{
			const ULyraExperienceDefinition* Experience = DefaultExperience.Get();
			if (Experience != nullptr)
			{
				// 触发数据验证（检查 Action 是否有效等）
				FDataValidationContext Context(EDataValidationUsecase::Save);
				DefaultExperience.LoadSynchronous();
				if (Experience->IsDataValid(Context) == EDataValidationResult::Invalid)
				{
					UE_LOG(LogLyraExperience, Error, TEXT("WorldSettings has invalid experience data"));
				}
			}
		}
	}
}
#endif
