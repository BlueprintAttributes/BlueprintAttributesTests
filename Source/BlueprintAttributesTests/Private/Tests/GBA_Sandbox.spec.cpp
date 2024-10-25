// Copyright 2022-2024 Mickael Daniel. All Rights Reserved.

#include "AttributeSet.h"
#include "Misc/AutomationTest.h"
#include "Misc/EngineVersionComparison.h"

#if UE_VERSION_OLDER_THAN(5, 5, 0)

// 5.4.x and down
inline constexpr uint8 EAutomationTestFlags_ApplicationContextMask = EAutomationTestFlags::EditorContext | EAutomationTestFlags::ClientContext | EAutomationTestFlags::ServerContext | EAutomationTestFlags::CommandletContext;

#endif

BEGIN_DEFINE_SPEC(FGBASandbox, "BlueprintAttributes.GBASandbox", EAutomationTestFlags::ProductFilter | EAutomationTestFlags_ApplicationContextMask)

	const FString FixtureLoadPath = TEXT("/BlueprintAttributesTests/Fixtures/GBAUtils_Spec/GBA_AttributeSetsUtils_Test.GBA_AttributeSetsUtils_Test_C");
	const FString FixtureAttributeSetLoadPath = TEXT("/BlueprintAttributesTests/Fixtures/GBAAttributeSetBlueprintBase_Spec/GBA_Test_Stats.GBA_Test_Stats_C");
	const FString FixtureClampAttributeSetLoadPath = TEXT("/BlueprintAttributesTests/Fixtures/GBAAttributeSetBlueprintBase_Spec/GBA_Test_Clamping.GBA_Test_Clamping_C");

	bool CheckStuff()
	{
		// Grab fixture Attribute Set class for further use later on
		const TSubclassOf<UAttributeSet> AttributeSetClass = StaticLoadClass(UAttributeSet::StaticClass(), nullptr, *FixtureAttributeSetLoadPath);
		if (!IsValid(AttributeSetClass))
		{
			AddError(FString::Printf(TEXT("Unable to load %s"), *FixtureAttributeSetLoadPath));
			return false;
		}

		TArray<FProperty*> AllProperties;
		for (TFieldIterator<FProperty> It(AttributeSetClass, EFieldIteratorFlags::ExcludeSuper); It; ++It)
		{
			FProperty* Property = *It;
			if (!Property)
			{
				continue;
			}
			
			AllProperties.Add(Property);
			AddInfo(FString::Printf(TEXT("Prop: %s"), *GetNameSafe(Property)));
		}
		
		return true;
	}

END_DEFINE_SPEC(FGBASandbox)

void FGBASandbox::Define()
{
	Describe(TEXT("GBASandbox"), [this]()
	{
		It(TEXT("Test stuff"), [this]()
		{
			TestTrue(TEXT("Check stuff"), CheckStuff());
		});
	});
}
