// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ue5_lyra : ModuleRules
{
	public ue5_lyra(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] 
		{ 
			"Core",   // 基础模块，包含日志、字符串等核心功能
			"CoreUObject",  // UObject 系统，支持反射、GC 等功能
			"Engine",   // 引擎模块，包含游戏逻辑、渲染等
			"InputCore",   // 输入系统，处理键盘、鼠标、手柄等输入
			"EnhancedInput",  // ★ 新增：UE5 输入系统，支持更灵活的输入映射和组合
			"GameplayAbilities",   // ★ 新增：GAS 核心(ASC/GA/GE/AttributeSet)
			"GameplayTags",        // ★ 新增：Tag 系统(InputTag.Fire 之类)
			"GameplayTasks",        // ★ 新增：GAS 异步任务(射击/蓄力等)
			"UMG"      // 控件系统（UserWidget）
		});

		PublicIncludePaths.AddRange(new string[] { ModuleDirectory });

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
