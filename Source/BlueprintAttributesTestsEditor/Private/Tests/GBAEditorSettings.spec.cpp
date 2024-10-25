// Copyright 2022-2024 Mickael Daniel. All Rights Reserved.

#include "GBAEditorSettings.h"
#include "Misc/AutomationTest.h"
#include "Misc/EngineVersionComparison.h"

#if UE_VERSION_OLDER_THAN(5, 5, 0)

// 5.4.x and down
inline constexpr uint8 EAutomationTestFlags_ApplicationContextMask = EAutomationTestFlags::EditorContext | EAutomationTestFlags::ClientContext | EAutomationTestFlags::ServerContext | EAutomationTestFlags::CommandletContext;

#endif

BEGIN_DEFINE_SPEC(FGBAEditorSettingsSpec, "BlueprintAttributes.Editor.GBAEditorSettings", EAutomationTestFlags::ProductFilter | EAutomationTestFlags_ApplicationContextMask)


END_DEFINE_SPEC(FGBAEditorSettingsSpec)

void FGBAEditorSettingsSpec::Define()
{
	Describe(TEXT("GBAEditorSettings::IsAttributeFiltered()"), [this]()
	{
		It(TEXT("should return same name when name is without trailing _C"), [this]()
		{
			TestTrue(
				TEXT("AbilitySystemComponent.OutgoingDuration filtered"),
				UGBAEditorSettings::IsAttributeFiltered(
					{TEXT("AbilitySystemComponent.OutgoingDuration"), TEXT("Foo")},
					TEXT("AbilitySystemComponent.OutgoingDuration")
				)
			);

			TestFalse(
				TEXT("AbilitySystemComponent.OutgoingDuration not filtered"),
				UGBAEditorSettings::IsAttributeFiltered(
					{TEXT("Foo")},
					TEXT("AbilitySystemComponent.OutgoingDuration")
				)
			);

			TestFalse(
				TEXT("AbilitySystemComponent.OutgoingDuration not filtered"),
				UGBAEditorSettings::IsAttributeFiltered(
					{TEXT("AbilitySysteaaamComponent.OutgoingDuration"), TEXT("Foo")},
					TEXT("AbilitySystemComponent.OutgoingDuration")
				)
			);

			TestTrue(
				TEXT("ExampleSet.SomeAttribute filtered"),
				UGBAEditorSettings::IsAttributeFiltered(
					{TEXT("ExampleSet")},
					TEXT("ExampleSet.SomeAttribute")
				)
			);

			TestTrue(
				TEXT("ExampleSet.SomeAttribute filtered"),
				UGBAEditorSettings::IsAttributeFiltered(
					{TEXT("ExampleSet.SomeAttribute")},
					TEXT("ExampleSet.SomeAttribute")
				)
			);

			TestTrue(
				TEXT("ExampleSet.SomeAttribute filtered"),
				UGBAEditorSettings::IsAttributeFiltered(
					{TEXT("ExampleSet.Some")},
					TEXT("ExampleSet.SomeAttribute")
				)
			);

			TestFalse(
				TEXT("ExampleSet.SomeAttribute not filtered"),
				UGBAEditorSettings::IsAttributeFiltered(
					{},
					TEXT("ExampleSet.SomeAttribute")
				)
			);
		});
	});
}
