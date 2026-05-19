// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Project_DSI : ModuleRules
{
	public Project_DSI(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "Niagara", "UMG", "EnhancedInput", "NavigationSystem", "AIModule", "GameplayTasks" });

		if (Target.bBuildEditor)
		{
			PublicDependencyModuleNames.AddRange(new string[] { "UnrealEd" });
        }
    }
}
