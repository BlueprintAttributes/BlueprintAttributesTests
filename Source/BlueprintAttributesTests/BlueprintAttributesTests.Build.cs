// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class BlueprintAttributesTests : ModuleRules
{
	public BlueprintAttributesTests(ReadOnlyTargetRules Target) : base(Target)
	{
		// PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		// ShortName = "GBATests";

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
			}
		);


		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"BlueprintAttributes",
				"BlueprintAttributesEditor",
				"BlueprintGraph",
				"CoreUObject",
				"Engine",
				"GameplayAbilities",
				"GameplayTags",
				"Slate",
				"SlateCore",
			}
		);
		
		if (Target.bBuildEditor)
		{
			PrivateDependencyModuleNames.AddRange(
				new string[]
				{
					"UnrealEd"
				}
			);
		}
	}
}