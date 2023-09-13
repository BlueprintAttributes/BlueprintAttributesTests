using UnrealBuildTool;

public class BlueprintAttributesTestsEditor : ModuleRules
{
    public BlueprintAttributesTestsEditor(ReadOnlyTargetRules Target) : base(Target)
    {
		ShortName = "GBATestsEditor";

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
				"BlueprintAttributesEditor",
                "BlueprintGraph",
                "CoreUObject",
                "Engine",
                "GameplayAbilities",
                "Slate",
                "SlateCore",
                "UnrealEd"
            }
        );
    }
}