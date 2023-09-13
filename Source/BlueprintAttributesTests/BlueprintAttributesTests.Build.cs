// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class BlueprintAttributesTests : ModuleRules
{
	public BlueprintAttributesTests(ReadOnlyTargetRules Target) : base(Target)
	{
		ShortName = "GBATests";

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
				"CoreUObject",
				"Engine",
				"GameplayAbilities",
				"GameplayTags",
				"Slate",
				"SlateCore",
			}
		);
	}
}