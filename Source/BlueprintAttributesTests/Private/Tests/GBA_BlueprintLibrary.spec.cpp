// Copyright 2022-2024 Mickael Daniel. All Rights Reserved.

#include "AttributeSet.h"
#include "Misc/AutomationTest.h"
#include "Misc/EngineVersionComparison.h"
#include "Utils/GBABlueprintLibrary.h"

#if UE_VERSION_OLDER_THAN(5, 5, 0)
#include "GBATestsFlags.h"
#endif

BEGIN_DEFINE_SPEC(FGBABlueprintLibrary, "BlueprintAttributes.GBABlueprintLibrary", EAutomationTestFlags::ProductFilter | EAutomationTestFlags_ApplicationContextMask)

	const FString FixtureAttributeSetLoadPath = TEXT("/BlueprintAttributesTests/Fixtures/GBAAttributeSetBlueprintBase_Spec/GBA_Test_Stats.GBA_Test_Stats_C");
	TSubclassOf<UAttributeSet> TestAttributeSetClass = nullptr;

	static FGameplayAttribute GetAttributeProperty(const UClass* InClass, const FName& InPropertyName)
	{
		return FindFProperty<FProperty>(InClass, InPropertyName);
	}

END_DEFINE_SPEC(FGBABlueprintLibrary)

void FGBABlueprintLibrary::Define()
{
	Describe(TEXT("GBABlueprintLibrary::GetDebugStringFromAttribute()"), [this]()
	{
		BeforeEach([this]()
		{
			// Grab fixture Attribute Set class for further use later on
			TestAttributeSetClass = StaticLoadClass(UAttributeSet::StaticClass(), nullptr, *FixtureAttributeSetLoadPath);
			if (!IsValid(TestAttributeSetClass))
			{
				AddError(FString::Printf(TEXT("Unable to load %s"), *FixtureAttributeSetLoadPath));
			}
		});

		It(TEXT("returns \"none\" empty on invalid attribute"), [this]()
		{
			const FGameplayAttribute Attribute;
			const FString Value = UGBABlueprintLibrary::GetDebugStringFromAttribute(Attribute);
			TestEqual(TEXT("Expected value is empty for invalid attribute"), Value, TEXT("none"));
		});

		It(TEXT("returns attribute name"), [this]()
		{
			const FName AttributeName = TEXT("Strength");
			const FGameplayAttribute Attribute = GetAttributeProperty(TestAttributeSetClass, AttributeName);
			if (!Attribute.IsValid())
			{
				AddError(FString::Printf(TEXT("Attribute %s is not valid (is it mispelled ?)"), *AttributeName.ToString()));
				return;
			}
			
			const FString Value = UGBABlueprintLibrary::GetDebugStringFromAttribute(Attribute);
			TestEqual(TEXT("Expected value is empty for invalid attribute"), Value, AttributeName.ToString());
		});
	});
}
