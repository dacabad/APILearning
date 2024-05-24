// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class APILearning : ModuleRules
{
	public APILearning(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		// @LESSON 
		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "OnlineSubsystem", "OnlineSubsystemSteam", 
		});
		
		PrivateDependencyModuleNames.AddRange(new string [] { "UMG", "OnlineImplementation" });
	}
}