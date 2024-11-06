// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class PCGTutoGit : ModuleRules
{
	public PCGTutoGit(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "PCG" });
	}
}
