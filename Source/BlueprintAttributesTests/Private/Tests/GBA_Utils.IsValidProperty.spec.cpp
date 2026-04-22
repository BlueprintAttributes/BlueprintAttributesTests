// Copyright 2022-2026 Mickael Daniel. All Rights Reserved.

#include "GBA_Utils.IsValidProperty.h"
#include "Misc/AutomationTest.h"
#include "Utils/GBAUtils.h"

BEGIN_DEFINE_SPEC(FGBAIsValidPropertySpec, "BlueprintAttributes.GBAUtils.IsValidProperty", EAutomationTestFlags::ProductFilter | EAutomationTestFlags_ApplicationContextMask)

END_DEFINE_SPEC(FGBAIsValidPropertySpec)

void FGBAIsValidPropertySpec::Define()
{
	Describe(TEXT("GBAUtils::IsValidProperty()"), [this]()
	{
		It(TEXT("returns true for FGameplayAttributeData properties"), [this]()
		{
			for (TFieldIterator<FProperty> It(UGBATestAttributeSet::StaticClass(), EFieldIteratorFlags::ExcludeSuper); It; ++It)
			{
				const FProperty* Property = *It;
				if (!Property)
				{
					continue;
				}

				AddInfo(FString::Printf(TEXT("Checking property: UGBATestAttributeSet.%s"), *Property->GetName()));
				TestTrue(TEXT("At least one valid FGameplayAttributeData property found"), FGBAUtils::IsValidProperty(Property));
			}
		});

		It(TEXT("returns false for null properties"), [this]()
		{
			TestFalse(TEXT("nullptr check"), FGBAUtils::IsValidProperty(nullptr));
		});

		It(TEXT("returns false for any properties not child of FGameplayAttributeData"), [this]()
		{
			for (TFieldIterator<FProperty> It(UGBATestDummy::StaticClass(), EFieldIteratorFlags::ExcludeSuper); It; ++It)
			{
				const FProperty* Property = *It;
				if (!Property)
				{
					continue;
				}

				AddInfo(FString::Printf(TEXT("Checking property: UGBATestDummy.%s"), *Property->GetName()));
				TestFalse(TEXT("not a FGameplayAttributeData property"), FGBAUtils::IsValidProperty(Property));
			}
		});
	});
}
