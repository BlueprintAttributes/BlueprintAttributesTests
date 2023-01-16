// Copyright 2021-2022 Mickael Daniel. All Rights Reserved.

#include "Misc/AutomationTest.h"

BEGIN_DEFINE_SPEC(FGBAEditorSubsystemSpec, "BlueprintAttributes.Editor.GBAEditorSubsystem", EAutomationTestFlags::ProductFilter | EAutomationTestFlags::ApplicationContextMask)

END_DEFINE_SPEC(FGBAEditorSubsystemSpec)

void FGBAEditorSubsystemSpec::Define()
{
	Describe(TEXT("Handle K2Nodes on Attribute rename"), [this]()
	{
		It(TEXT("should update all K2 Nodes with a FGameplayAttribute Pin parameter"), [this]()
		{
			AddInfo(FString::Printf(TEXT("Test stuff here")));
		});
	});
}
