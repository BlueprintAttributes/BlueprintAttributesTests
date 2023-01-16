// Copyright 2021-2022 Mickael Daniel. All Rights Reserved.

#include "AbilitySystemComponent.h"
#include "AttributeSet.h"
#include "Abilities/GBAAttributeSetBlueprintBase.h"
#include "GBAAttributeSetSpecBase.h"
#include "GameFramework/Character.h"
#include "Misc/AutomationTest.h"

GBA_BEGIN_DEFINE_SPEC_WITH_BASE(FGBAAttributeSetAttributeClampingHealthSpec, FGBAAttributeSetSpecBase, "BlueprintAttributes.GBAAttributeSetBlueprintBase.Clamping", EAutomationTestFlags::ProductFilter | EAutomationTestFlags::ApplicationContextMask)
	
	static constexpr const TCHAR* FixtureClampHealthAttributeSetLoadPath = TEXT("/BlueprintAttributesTests/Fixtures/AttributeBasedClamping/GBA_Test_HealthSet.GBA_Test_HealthSet_C");
	static constexpr const TCHAR* FixtureGameplayEffectDamagePath = TEXT("/BlueprintAttributesTests/Fixtures/AttributeBasedClamping/GE_Test_Damage.GE_Test_Damage_C");

GBA_END_DEFINE_SPEC(FGBAAttributeSetAttributeClampingHealthSpec)

void FGBAAttributeSetAttributeClampingHealthSpec::Define()
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

	Describe(TEXT("Clamping (Attribute Based - Health)"), [this]()
	{
		BeforeEach([this]()
		{
			// Grab fixture Attribute Set class for further use later on
			TestAttributeSetClass = StaticLoadClass(UAttributeSet::StaticClass(), nullptr, FixtureClampHealthAttributeSetLoadPath);
			if (!IsValid(TestAttributeSetClass))
			{
				AddError(FString::Printf(TEXT("Unable to load %s"), FixtureClampHealthAttributeSetLoadPath));
				return;
			}

			TestASC->InitStats(TestAttributeSetClass, nullptr);

			// We need the prop to be non const (GetAttributeSet returns const value) for some of the API testing below on non const functions
			TestAttributeSet = Cast<UGBAAttributeSetBlueprintBase>(const_cast<UAttributeSet*>(TestASC->GetAttributeSet(TestAttributeSetClass)));
			if (!TestAttributeSet)
			{
				AddError(FString::Printf(TEXT("Couldn't get attribute set or cast to UGBAAttributeSetBlueprintBase")));
			}
		});

		It(TEXT("should have attributes clamped after Gameplay Effect application"), [this]()
		{
			TestAttributeClamping({ TEXT("Health"), 0.f }, { TEXT("MinHealth"), -20.f }, { TEXT("MaxHealth"), 80.f });
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
