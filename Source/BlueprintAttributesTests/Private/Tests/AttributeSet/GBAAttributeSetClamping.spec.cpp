﻿// Copyright 2021-2022 Mickael Daniel. All Rights Reserved.

#include "AbilitySystemComponent.h"
#include "AttributeSet.h"
#include "Abilities/GBAAttributeSetBlueprintBase.h"
#include "GBAAttributeSetSpecBase.h"
#include "GameFramework/Character.h"
#include "Misc/AutomationTest.h"

GBA_BEGIN_DEFINE_SPEC_WITH_BASE(FGBAAttributeSetClampingSpec, FGBAAttributeSetSpecBase, "BlueprintAttributes.GBAAttributeSetBlueprintBase.Clamping", EAutomationTestFlags::ProductFilter | EAutomationTestFlags::ApplicationContextMask)

	static constexpr const TCHAR* FixtureClampAttributeSetLoadPath = TEXT("/BlueprintAttributesTests/Tests/Fixtures/GBAAttributeSetBlueprintBase_Spec/GBA_Test_Clamping.GBA_Test_Clamping_C");
	static constexpr const TCHAR* FixtureGameplayEffectAddLoadPath = TEXT("/BlueprintAttributesTests/Tests/Fixtures/GBAAttributeSetBlueprintBase_Spec/GE_Test_Clamped_Add.GE_Test_Clamped_Add_C");
	static constexpr const TCHAR* FixtureGameplayEffectSubLoadPath = TEXT("/BlueprintAttributesTests/Tests/Fixtures/GBAAttributeSetBlueprintBase_Spec/GE_Test_Clamped_Substract.GE_Test_Clamped_Substract_C");

GBA_END_DEFINE_SPEC(FGBAAttributeSetClampingSpec)

void FGBAAttributeSetClampingSpec::Define()
{
	BeforeEach([this]()
	{
		AddInfo(TEXT("Before Each ..."));

		// Setup tests
		World = CreateWorld(InitialFrameCounter);

		UClass* ActorClass = StaticLoadClass(UObject::StaticClass(), nullptr, FixtureCharacterLoadPath);
		if (!IsValid(ActorClass))
		{
			AddError(FString::Printf(TEXT("Unable to load %s"), FixtureCharacterLoadPath));
			return;
		}

		// set up the destination actor
		TestActor = Cast<ACharacter>(World->SpawnActor(ActorClass, nullptr, nullptr, FActorSpawnParameters()));
		if (!TestActor)
		{
			AddError(FString::Printf(TEXT("Unable to setup test actor from %s"), *GetNameSafe(ActorClass)));
			return;
		}

		// set up the source actor
		TestASC = TestActor->FindComponentByClass<UAbilitySystemComponent>();
		if (!TestASC)
		{
			AddError(FString::Printf(TEXT("Unable to get ASC from test actor %s"), *GetNameSafe(TestActor)));
			return;
		}

		// Make sure BeginPlay is invoked (this is where fixture char is granting attributes)
		TestActor->DispatchBeginPlay();
	});

	Describe(TEXT("Clamping (via DataTable or Clamped Struct Defaults)"), [this]()
	{
		BeforeEach([this]()
		{
			// Grab fixture Attribute Set class for further use later on
			TestAttributeSetClass = StaticLoadClass(UAttributeSet::StaticClass(), nullptr, FixtureClampAttributeSetLoadPath);
			if (!IsValid(TestAttributeSetClass))
			{
				AddError(FString::Printf(TEXT("Unable to load %s"), FixtureClampAttributeSetLoadPath));
				return;
			}

			// Grant (it is not done from within Character's Blueprint Begin Play)
			const UDataTable* DataTable = StaticLoadDataTable(TEXT("/BlueprintAttributesTests/Tests/Fixtures/GBAAttributeSetBlueprintBase_Spec/DT_Test_Clamp"));
			if (!DataTable)
			{
				AddError(FString::Printf(TEXT("Unable to load clamped data table")));
				return;
			}

			TestASC->InitStats(TestAttributeSetClass, DataTable);

			// We need the prop to be non const (GetAttributeSet returns const value) for some of the API testing below on non const functions
			TestAttributeSet = Cast<UGBAAttributeSetBlueprintBase>(const_cast<UAttributeSet*>(TestASC->GetAttributeSet(TestAttributeSetClass)));
			if (!TestAttributeSet)
			{
				AddError(FString::Printf(TEXT("Couldn't get attribute set or cast to UGBAAttributeSetBlueprintBase")));
			}
		});

		It(TEXT("has attributes meta map initialized"), [this]()
		{
			const TMap<FString, TSharedPtr<FAttributeMetaData>> AttributesMetaData = TestAttributeSet->GetAttributesMetaData();

			TestFalse(TEXT("Attributes meta data not empty"), AttributesMetaData.IsEmpty());

			const TArray<FString> Attributes = {TEXT("TestDTClamp"), TEXT("TestBoth")};
			for (const FString& Attribute : Attributes)
			{
				const bool bHasMetaData = AttributesMetaData.Contains(Attribute);
				TestTrue(FString::Printf(TEXT("Attributes meta data contains entry for %s"), *Attribute), AttributesMetaData.Contains(Attribute));

				if (!bHasMetaData)
				{
					continue;
				}

				TSharedPtr<FAttributeMetaData> MetaData = AttributesMetaData.FindChecked(Attribute);
				if (!MetaData.IsValid())
				{
					AddError(FString::Printf(TEXT("Metadata shared ptr invalid for %s"), *Attribute));
					continue;
				}

				TestEqual(TEXT("Metadata Base value"), MetaData->BaseValue, 1000.f);
				TestEqual(TEXT("Metadata Min value"), MetaData->MinValue, -100.f);
				TestEqual(TEXT("Metadata Max value"), MetaData->MaxValue, 100.f);
			}
		});

		It(TEXT("should have base value clamped after DT initialization"), [this]()
		{
			// Datable base values were set to 1000.f but clamping should have maxed it to 100.f
			TestAttribute(TEXT("TestDTClamp"), 100.f);
			TestAttribute(TEXT("TestBoth"), 100.f);

			// This one was not part of the Datatable
			TestAttribute(TEXT("TestClampedAttribute"), 0.f);

			// This was as well
			TestAttribute(TEXT("TestNotClamped"), 0.f);
		});

		It(TEXT("should have attributes clamped after Gameplay Effect application"), [this]()
		{
			TestAttribute(TEXT("TestDTClamp"), 100.f);
			TestAttribute(TEXT("TestBoth"), 100.f);
			TestAttribute(TEXT("TestClampedAttribute"), 0.f);
			TestAttribute(TEXT("TestNotClamped"), 0.f);

			// Effect has a -10000 modifier for each
			ApplyGameplayEffect(TestASC, FixtureGameplayEffectSubLoadPath);

			TestAttribute(TEXT("TestDTClamp"), -100.f);
			TestAttribute(TEXT("TestBoth"), -10.f);
			TestAttribute(TEXT("TestClampedAttribute"), -10.f);
			TestAttribute(TEXT("TestNotClamped"), -10000.f);

			// Effect has a +100000 modifier for each
			const FGameplayAttribute Attribute = GetAttributeProperty(TestAttributeSetClass, TEXT("TestClampedAttribute"));
			AddInfo(FString::Printf(TEXT("Value before GE Current: %f (Base: %f) - %s"), TestASC->GetNumericAttribute(Attribute), TestASC->GetNumericAttributeBase(Attribute), *Attribute.GetName()));
			ApplyGameplayEffect(TestASC, FixtureGameplayEffectAddLoadPath);
			AddInfo(FString::Printf(TEXT("Value after GE Current: %f (Base: %f) - %s"), TestASC->GetNumericAttribute(Attribute), TestASC->GetNumericAttributeBase(Attribute), *Attribute.GetName()));

			TestAttribute(TEXT("TestDTClamp"), 100.f);
			TestAttribute(TEXT("TestBoth"), 10.f);
			TestAttribute(TEXT("TestClampedAttribute"), 10.f);
			TestAttribute(TEXT("TestNotClamped"), 90000.f);
		});
	});

	AfterEach([this]()
	{
		AddInfo(TEXT("After Each ..."));

		// Destroy the actors
		if (TestActor)
		{
			World->EditorDestroyActor(TestActor, false);
		}

		// Destroy World
		TeardownWorld(World, InitialFrameCounter);
	});
}
