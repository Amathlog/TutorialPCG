// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class PCGTutoGit_5_3 : ModuleRules
{
	public PCGTutoGit_5_3(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput" });
	}
}
