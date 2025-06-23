// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class AnimReplicatedRagdollEditor : ModuleRules
{
	public AnimReplicatedRagdollEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"AnimGraph",
                "BlueprintGraph",
				"AnimReplicatedRagdoll",
			}
		);
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
			}
		);
	}
}
