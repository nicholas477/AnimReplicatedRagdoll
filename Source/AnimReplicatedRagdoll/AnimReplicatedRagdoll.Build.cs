// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class AnimReplicatedRagdoll : ModuleRules
{
	public AnimReplicatedRagdoll(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"DeveloperSettings"
			}
		);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
                "NetCore"
            }
		);
	}
}
