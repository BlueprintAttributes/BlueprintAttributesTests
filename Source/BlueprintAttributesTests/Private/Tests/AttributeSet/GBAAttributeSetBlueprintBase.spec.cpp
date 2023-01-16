// Copyright 2021-2022 Mickael Daniel. All Rights Reserved.

#include "AbilitySystemComponent.h"
#include "AttributeSet.h"
#include "GBATestsNativeTags.h"
#include "GBATestsStorageSubsystem.h"
#include "Abilities/GBAAttributeSetBlueprintBase.h"
#include "Animation/AnimInstance.h"
#include "GBAAttributeSetSpecBase.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/MovementComponent.h"
#include "Misc/AutomationTest.h"

GBA_BEGIN_DEFINE_SPEC_WITH_BASE(FGBAAttributeSetBlueprintBaseSpec, FGBAAttributeSetSpecBase, "BlueprintAttributes.GBAAttributeSetBlueprintBase", EAutomationTestFlags::ProductFilter | EAutomationTestFlags::ApplicationContextMask)
	UGBAAttributeSetBlueprintBase* TestAttributeSet = nullptr;
	
	static constexpr const TCHAR* FixtureAttributeSetLoadPath = TEXT("/BlueprintAttributesTests/Fixtures/GBAAttributeSetBlueprintBase_Spec/GBA_Test_Stats.GBA_Test_Stats_C");
	static constexpr const TCHAR* FixtureGameplayEffectLoadPath = TEXT("/BlueprintAttributesTests/Fixtures/GBAAttributeSetBlueprintBase_Spec/GE_Test_Stats_Init.GE_Test_Stats_Init_C");
GBA_END_DEFINE_SPEC(FGBAAttributeSetBlueprintBaseSpec)

void FGBAAttributeSetBlueprintBaseSpec::Define()
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

		// Grab fixture Attribute Set class for further use later on
		TestAttributeSetClass = StaticLoadClass(UAttributeSet::StaticClass(), nullptr, FixtureAttributeSetLoadPath);
		if (!IsValid(TestAttributeSetClass))
		{
			AddError(FString::Printf(TEXT("Unable to load %s"), FixtureAttributeSetLoadPath));
			return;
		}

		// We need the prop to be non const (GetAttributeSet returns const value) for some of the API testing below on non const functions
		TestAttributeSet = Cast<UGBAAttributeSetBlueprintBase>(const_cast<UAttributeSet*>(TestASC->GetAttributeSet(TestAttributeSetClass)));
		if (!TestAttributeSet)
		{
			AddError(FString::Printf(TEXT("Couldn't get attribute set or cast to UGBAAttributeSetBlueprintBase")));
		}
	});

	Describe(TEXT("Sanity Check"), [this]()
	{
		// Not implemented, might be removed in favor of a FGBAAttributeSetExecutionData ctor
		It(TEXT("should spawned actor be initialized with an AbilitySystemComponent"), [this]()
		{
			const UAbilitySystemComponent* ASC = TestActor->FindComponentByClass<UAbilitySystemComponent>();
			TestTrue("Source Actor has UGSCAbilitySystemComponent", ASC != nullptr);
		});

		It(TEXT("should have AttributeSet granted and initialized"), [this]()
		{
			TestFalse("Test ASC has no stock UAttributeSet granted", HasAttributeSet(TestASC, UAttributeSet::StaticClass()));
			TestTrue("Test ASC has fixture BP AttributeSet granted", HasAttributeSet(TestASC, TestAttributeSetClass));
		});

		It(TEXT("should have Attributes values initialized to 0"), [this]()
		{
			// Those Attributes are defined in the BP /BlueprintAttributesTests/Tests/Fixtures/GBAAttributeSetBlueprintBase_Spec/GBA_Test_Stats
			TestAttribute(TEXT("Vitality"));
			TestAttribute(TEXT("Endurance"));
			TestAttribute(TEXT("Strength"));
			TestAttribute(TEXT("Dexterity"));
			TestAttribute(TEXT("Intelligence"));
			TestAttribute(TEXT("Faith"));
			TestAttribute(TEXT("Luck"));
		});

		It(TEXT("should have value changed after application of override instant GE"), [this]()
		{
			// Grab fixture Attribute Set class for further use later on
			const TSubclassOf<UGameplayEffect> Effect = StaticLoadClass(UGameplayEffect::StaticClass(), nullptr, FixtureGameplayEffectLoadPath);
			if (!IsValid(Effect))
			{
				AddError(FString::Printf(TEXT("Unable to load %s"), FixtureGameplayEffectLoadPath));
				return;
			}

			TestASC->BP_ApplyGameplayEffectToSelf(Effect, 1.f, TestASC->MakeEffectContext());

			TestAttribute(TEXT("Vitality"), 10.f);
			TestAttribute(TEXT("Endurance"), 10.f);
			TestAttribute(TEXT("Strength"), 10.f);
			TestAttribute(TEXT("Dexterity"), 10.f);
			TestAttribute(TEXT("Intelligence"), 10.f);
			TestAttribute(TEXT("Faith"), 10.f);
			TestAttribute(TEXT("Luck"), 10.f);
		});
	});

	Describe(TEXT("API"), [this]()
	{
		BeforeEach([this]()
		{
			GetStorage().ResetStore();
		});

		It(TEXT("UGBAAttributeSetBlueprintBase::GetOwningActor()"), [this]()
		{
			check(TestAttributeSet);

			const AActor* OwningActor = TestAttributeSet->K2_GetOwningActor();
			AddInfo(FString::Printf(TEXT("OwningActor: %s"), *GetNameSafe(OwningActor)));
			TestTrue(TEXT("GetOwningActor() returns correct actor"), OwningActor == TestActor);
		});

		It(TEXT("UGBAAttributeSetBlueprintBase::GetOwningAbilitySystemComponent()"), [this]()
		{
			check(TestAttributeSet);

			const UAbilitySystemComponent* OwningASC = TestAttributeSet->K2_GetOwningAbilitySystemComponent();
			AddInfo(FString::Printf(TEXT("OwningASC: %s"), *GetNameSafe(OwningASC)));
			TestTrue(TEXT("GetOwningActor() returns correct actor"), OwningASC == TestASC);
		});

		It(TEXT("UGBAAttributeSetBlueprintBase::GetActorInfo()"), [this]()
		{
			check(TestAttributeSet);

			const FGameplayAbilityActorInfo ActorInfo = TestAttributeSet->K2_GetActorInfo();
			AddInfo(FString::Printf(TEXT("ActorInfo AvatarActor: %s"), *GetNameSafe(ActorInfo.AvatarActor.Get())));
			AddInfo(FString::Printf(TEXT("ActorInfo OwnerActor: %s"), *GetNameSafe(ActorInfo.OwnerActor.Get())));
			AddInfo(FString::Printf(TEXT("ActorInfo AbilitySystemComponent: %s"), *GetNameSafe(ActorInfo.AbilitySystemComponent.Get())));
			AddInfo(FString::Printf(TEXT("ActorInfo SkeletalMeshComponent: %s"), *GetNameSafe(ActorInfo.SkeletalMeshComponent.Get())));
			AddInfo(FString::Printf(TEXT("ActorInfo AnimInstance: %s"), *GetNameSafe(ActorInfo.AnimInstance.Get())));
			AddInfo(FString::Printf(TEXT("ActorInfo MovementComponent: %s"), *GetNameSafe(ActorInfo.MovementComponent.Get())));

			TestTrue(TEXT("ActorInfo AvatarActor"), ActorInfo.AvatarActor.Get() == TestActor);
			TestTrue(TEXT("ActorInfo OwnerActor"), ActorInfo.OwnerActor.Get() == TestActor);
			TestTrue(TEXT("ActorInfo AbilitySystemComponent"), ActorInfo.AbilitySystemComponent.Get() == TestASC);
			TestTrue(TEXT("ActorInfo SkeletalMeshComponent"), ActorInfo.SkeletalMeshComponent.Get() == TestActor->GetMesh());

			if (!TestActor->GetMesh())
			{
				AddError(FString::Printf(TEXT("Couldn't get mesh from TestActor")));
				return;
			}

			AddInfo(FString::Printf(TEXT("TestActor AnimInstance: %s"), *GetNameSafe(TestActor->GetMesh()->GetAnimInstance())));

			AddInfo(FString::Printf(TEXT("TestActor CharacterMovementComponent: %s"), *GetNameSafe(TestActor->GetCharacterMovement())));
			TestTrue(TEXT("ActorInfo MovementComponent"), ActorInfo.MovementComponent.Get() == TestActor->GetCharacterMovement());
		});

		It(TEXT("UGBAAttributeSetBlueprintBase::GetAttributeValue()"), [this]()
		{
			auto TestAttributeValue = [this](const FName& InAttributeName, const float InExpected) -> void
			{
				const FGameplayAttribute Attribute = GetAttributeProperty(TestAttributeSetClass, InAttributeName);
				const float Value = TestAttributeSet->GetAttributeValue(Attribute);
				TestEqual(FString::Printf(TEXT("GetAttributeValue: %s check"), *InAttributeName.ToString()), Value, InExpected);
			};

			TestAttributeValue(TEXT("Vitality"), 0.f);
			TestAttributeValue(TEXT("Endurance"), 0.f);
			TestAttributeValue(TEXT("Strength"), 0.f);
			TestAttributeValue(TEXT("Dexterity"), 0.f);
			TestAttributeValue(TEXT("Intelligence"), 0.f);
			TestAttributeValue(TEXT("Faith"), 0.f);
			TestAttributeValue(TEXT("Luck"), 0.f);

			const TSubclassOf<UGameplayEffect> Effect = StaticLoadClass(UGameplayEffect::StaticClass(), nullptr, FixtureGameplayEffectLoadPath);
			if (!IsValid(Effect))
			{
				AddError(FString::Printf(TEXT("Unable to load %s"), FixtureGameplayEffectLoadPath));
				return;
			}

			TestASC->BP_ApplyGameplayEffectToSelf(Effect, 1.f, TestASC->MakeEffectContext());

			TestAttributeValue(TEXT("Vitality"), 10.f);
			TestAttributeValue(TEXT("Endurance"), 10.f);
			TestAttributeValue(TEXT("Strength"), 10.f);
			TestAttributeValue(TEXT("Dexterity"), 10.f);
			TestAttributeValue(TEXT("Intelligence"), 10.f);
			TestAttributeValue(TEXT("Faith"), 10.f);
			TestAttributeValue(TEXT("Luck"), 10.f);
		});

		It(TEXT("UGBAAttributeSetBlueprintBase::SetAttributeValue()"), [this]()
		{
			TestAttribute(TEXT("Vitality"), 0.f);

			const FGameplayAttribute Attribute = GetAttributeProperty(TestAttributeSetClass, TEXT("Vitality"));
			TestAttributeSet->SetAttributeValue(Attribute, 99.f);
			TestAttribute(TEXT("Vitality"), 99.f);

			// Over 9000
			TestAttributeSet->K2_SetAttributeValue(Attribute, 9001.f);
			TestAttribute(TEXT("Vitality"), 9001.f);
		});

		It(TEXT("UGBAAttributeSetBlueprintBase::ClampAttributeValue()"), [this]()
		{
			TestAttribute(TEXT("Vitality"), 0.f);

			const FGameplayAttribute Attribute = GetAttributeProperty(TestAttributeSetClass, TEXT("Vitality"));

			// Clamps value to higher bound
			TestAttributeSet->SetAttributeValue(Attribute, 99.f);
			TestAttribute(TEXT("Vitality"), 99.f);

			TestAttributeSet->ClampAttributeValue(Attribute, 0.f, 25.f);
			TestAttribute(TEXT("Vitality"), 25.f);

			// Leave value as is if within the bounds
			TestAttributeSet->SetAttributeValue(Attribute, 12.f);
			TestAttribute(TEXT("Vitality"), 12.f);

			TestAttributeSet->ClampAttributeValue(Attribute, 0.f, 25.f);
			TestAttribute(TEXT("Vitality"), 12.f);

			// Clamps value to lower bound
			TestAttributeSet->SetAttributeValue(Attribute, -99.f);
			TestAttribute(TEXT("Vitality"), -99.f);

			TestAttributeSet->ClampAttributeValue(Attribute, 0.f, 25.f);
			TestAttribute(TEXT("Vitality"), 0.f);
		});

		It(TEXT("UGBAAttributeSetBlueprintBase::InitFromMetaDataTable"), [this]()
		{
			TestAttribute(TEXT("Vitality"), 0.f);
			TestAttribute(TEXT("Endurance"), 0.f);
			TestAttribute(TEXT("Strength"), 0.f);
			TestAttribute(TEXT("Dexterity"), 0.f);
			TestAttribute(TEXT("Intelligence"), 0.f);
			TestAttribute(TEXT("Faith"), 0.f);
			TestAttribute(TEXT("Luck"), 0.f);

			const UDataTable* DataTable = StaticLoadDataTable(TEXT("/BlueprintAttributesTests/Fixtures/GBAAttributeSetBlueprintBase_Spec/DT_Test_Stats"));
			TestAttributeSet->InitFromMetaDataTable(DataTable);

			TestAttribute(TEXT("Vitality"), 12.f);
			TestAttribute(TEXT("Endurance"), 12.f);
			TestAttribute(TEXT("Strength"), 12.f);
			TestAttribute(TEXT("Dexterity"), 12.f);
			TestAttribute(TEXT("Intelligence"), 12.f);
			TestAttribute(TEXT("Faith"), 12.f);
			TestAttribute(TEXT("Luck"), 12.f);
		});

		It(TEXT("UGBAAttributeSetBlueprintBase::PreGameplayEffectExecute"), [this]()
		{
			TMap<FName, FGameplayAttribute> StorageKeys = GetStorageTestMap(TEXT("PreGameplayEffectExecute"));

			// Check storage is empty before apply effect
			for (const TPair<FName, FGameplayAttribute>& Entry : StorageKeys)
			{
				FName StorageKey = Entry.Key;
				FGBATestStorage_PreGameplayEffectExecutePayload Payload = GetStorage().GetValueAsPreGameplayEffectExecutePayload(StorageKey);
				AddInfo(FString::Printf(TEXT("Checking storage for %s is invalid as expected"), *StorageKey.ToString()));
				TestFalse(FString::Printf(TEXT("Storage for %s is not initialized yet"), *StorageKey.ToString()), Payload.Attribute.IsValid());
			}

			// Apply effect, this should kick in BP events implemented in fixture BP AttributeSet
			ApplyGameplayEffect(TestASC, FixtureGameplayEffectLoadPath);

			// Check storage is filled after apply effect
			//
			// Fixture Attribute Set BP is implementing PostGameplayEffectExecute event and stores whatever it gets into the storage
			// so that we can check here it has been called, and with the proper parameters
			for (const TPair<FName, FGameplayAttribute>& Entry : StorageKeys)
			{
				FName StorageKey = Entry.Key;
				FGameplayAttribute Attribute = Entry.Value;
				FGBATestStorage_PreGameplayEffectExecutePayload Payload = GetStorage().GetValueAsPreGameplayEffectExecutePayload(StorageKey);
				AddInfo(FString::Printf(TEXT("Checking storage for %s was set as expected"), *StorageKey.ToString()));
				TestEqual(FString::Printf(TEXT("Storage for %s was initialized"), *StorageKey.ToString()), Payload.Attribute, Attribute);

				TestTrue(TEXT("ExecData SourceActor"), Payload.ExecData.SourceActor == TestActor);
				TestTrue(TEXT("ExecData TargetActor"), Payload.ExecData.TargetActor == TestActor);
				TestTrue(TEXT("ExecData SourceASC"), Payload.ExecData.SourceASC == TestASC);
				TestTrue(TEXT("ExecData TargetASC"), Payload.ExecData.TargetASC == TestASC);
				// Test Possessing TestActor ? or maybe setup functional tests for this
				TestTrue(TEXT("ExecData SourceController"), Payload.ExecData.SourceController == nullptr);
				TestTrue(TEXT("ExecData TargetController"), Payload.ExecData.TargetController == nullptr);

				// Tags ?
				FGameplayTagContainer Container;
				Container.AddTag(FGBATestsNativeTags::Get().TestEffect);
				TestEqual(TEXT("ExecData SourceTags"), Payload.ExecData.SourceTags, Container);
				TestEqual(TEXT("ExecData SpecAssetTags"), Payload.ExecData.SpecAssetTags, Container);

				TestEqual(TEXT("ExecData DeltaValue"), Payload.ExecData.DeltaValue, 0.f);
				TestEqual(TEXT("ExecData DeltaValue"), Payload.ExecData.MagnitudeValue, 10.f);
			}
		});

		It(TEXT("UGBAAttributeSetBlueprintBase::PostGameplayEffectExecute"), [this]()
		{
			TMap<FName, FGameplayAttribute> StorageKeys = GetStorageTestMap(TEXT("PostGameplayEffectExecute"));

			// Check storage is empty before apply effect
			for (const TPair<FName, FGameplayAttribute>& Entry : StorageKeys)
			{
				FName StorageKey = Entry.Key;
				FGBATestStorage_PostGameplayEffectExecutePayload Payload = GetStorage().GetValueAsPostGameplayEffectExecutePayload(StorageKey);
				AddInfo(FString::Printf(TEXT("Checking storage for %s is invalid as expected"), *StorageKey.ToString()));
				TestFalse(FString::Printf(TEXT("Storage for %s is not initialized yet"), *StorageKey.ToString()), Payload.Attribute.IsValid());
			}

			// Apply effect, this should kick in BP events implemented in fixture BP AttributeSet
			ApplyGameplayEffect(TestASC, FixtureGameplayEffectLoadPath);

			// Check storage is filled after apply effect
			//
			// Fixture Attribute Set BP is implementing PostGameplayEffectExecute event and stores whatever it gets into the storage
			// so that we can check here it has been called, and with the proper parameters
			for (const TPair<FName, FGameplayAttribute>& Entry : StorageKeys)
			{
				FName StorageKey = Entry.Key;
				FGameplayAttribute Attribute = Entry.Value;
				FGBATestStorage_PostGameplayEffectExecutePayload Payload = GetStorage().GetValueAsPostGameplayEffectExecutePayload(StorageKey);
				AddInfo(FString::Printf(TEXT("Checking storage for %s was set as expected"), *StorageKey.ToString()));
				TestEqual(FString::Printf(TEXT("Storage for %s was initialized"), *StorageKey.ToString()), Payload.Attribute, Attribute);

				TestTrue(TEXT("ExecData SourceActor"), Payload.ExecData.SourceActor == TestActor);
				TestTrue(TEXT("ExecData TargetActor"), Payload.ExecData.TargetActor == TestActor);
				TestTrue(TEXT("ExecData SourceASC"), Payload.ExecData.SourceASC == TestASC);
				TestTrue(TEXT("ExecData TargetASC"), Payload.ExecData.TargetASC == TestASC);
				// Test Possessing TestActor ? or maybe setup functional tests for this
				TestTrue(TEXT("ExecData SourceController"), Payload.ExecData.SourceController == nullptr);
				TestTrue(TEXT("ExecData TargetController"), Payload.ExecData.TargetController == nullptr);

				// Tags ?
				FGameplayTagContainer Container;
				Container.AddTag(FGBATestsNativeTags::Get().TestEffect);
				TestEqual(TEXT("ExecData SourceTags"), Payload.ExecData.SourceTags, Container);
				TestEqual(TEXT("ExecData SpecAssetTags"), Payload.ExecData.SpecAssetTags, Container);

				TestEqual(TEXT("ExecData DeltaValue"), Payload.ExecData.DeltaValue, 0.f);
				TestEqual(TEXT("ExecData DeltaValue"), Payload.ExecData.MagnitudeValue, 10.f);
			}
		});

		It(TEXT("UGBAAttributeSetBlueprintBase::PreAttributeChange"), [this]()
		{
			TMap<FName, FGameplayAttribute> StorageKeys = GetStorageTestMap(TEXT("PreAttributeChange"));

			// Check storage is empty before apply effect
			for (const TPair<FName, FGameplayAttribute>& Entry : StorageKeys)
			{
				FName StorageKey = Entry.Key;
				FGBATestStorage_PreAttributeChangePayload Payload = GetStorage().GetValueAsPreAttributeChangePayload(StorageKey);
				AddInfo(FString::Printf(TEXT("Checking storage for %s is invalid as expected"), *StorageKey.ToString()));
				TestFalse(FString::Printf(TEXT("Storage for %s is not initialized yet"), *StorageKey.ToString()), Payload.Attribute.IsValid());
			}

			// Apply effect, this should kick in BP events implemented in fixture BP AttributeSet
			ApplyGameplayEffect(TestASC, FixtureGameplayEffectLoadPath);

			// Check storage is filled after apply effect
			//
			// Fixture Attribute Set BP is implementing PostGameplayEffectExecute event and stores whatever it gets into the storage
			// so that we can check here it has been called, and with the proper parameters
			for (const TPair<FName, FGameplayAttribute>& Entry : StorageKeys)
			{
				FName StorageKey = Entry.Key;
				FGameplayAttribute Attribute = Entry.Value;
				FGBATestStorage_PreAttributeChangePayload Payload = GetStorage().GetValueAsPreAttributeChangePayload(StorageKey);
				AddInfo(FString::Printf(TEXT("Checking storage for %s was set as expected"), *StorageKey.ToString()));
				TestEqual(FString::Printf(TEXT("Storage for %s was initialized"), *StorageKey.ToString()), Payload.Attribute, Attribute);
				TestEqual(TEXT("Check payload value"), Payload.Value, 10.f);
			}
		});

		It(TEXT("UGBAAttributeSetBlueprintBase::PostAttributeChange"), [this]()
		{
			TMap<FName, FGameplayAttribute> StorageKeys = GetStorageTestMap(TEXT("PostAttributeChange"));

			// Check storage is empty before apply effect
			for (const TPair<FName, FGameplayAttribute>& Entry : StorageKeys)
			{
				FName StorageKey = Entry.Key;
				FGBATestStorage_PostAttributeChangePayload Payload = GetStorage().GetValueAsPostAttributeChangePayload(StorageKey);
				AddInfo(FString::Printf(TEXT("Checking storage for %s is invalid as expected"), *StorageKey.ToString()));
				TestFalse(FString::Printf(TEXT("Storage for %s is not initialized yet"), *StorageKey.ToString()), Payload.Attribute.IsValid());
			}

			// Apply effect, this should kick in BP events implemented in fixture BP AttributeSet
			for (const TPair<FName, FGameplayAttribute>& Entry : StorageKeys)
			{
				FGameplayAttribute Attribute = Entry.Value;
				AddInfo(FString::Printf(TEXT("Value before GE Current: %f (Base: %f) - %s"), TestASC->GetNumericAttribute(Attribute), TestASC->GetNumericAttributeBase(Attribute), *Attribute.GetName()));
			}
			ApplyGameplayEffect(TestASC, FixtureGameplayEffectLoadPath);
			for (const TPair<FName, FGameplayAttribute>& Entry : StorageKeys)
			{
				FGameplayAttribute Attribute = Entry.Value;
				AddInfo(FString::Printf(TEXT("Value after GE Current: %f (Base: %f) - %s"), TestASC->GetNumericAttribute(Attribute), TestASC->GetNumericAttributeBase(Attribute), *Attribute.GetName()));
			}

			// Check storage is filled after apply effect
			//
			// Fixture Attribute Set BP is implementing PostGameplayEffectExecute event and stores whatever it gets into the storage
			// so that we can check here it has been called, and with the proper parameters
			for (const TPair<FName, FGameplayAttribute>& Entry : StorageKeys)
			{
				FName StorageKey = Entry.Key;
				FGameplayAttribute Attribute = Entry.Value;
				FGBATestStorage_PostAttributeChangePayload Payload = GetStorage().GetValueAsPostAttributeChangePayload(StorageKey);
				AddInfo(FString::Printf(TEXT("Checking storage for %s was set as expected"), *StorageKey.ToString()));
				TestEqual(FString::Printf(TEXT("Storage for %s was initialized"), *StorageKey.ToString()), Payload.Attribute, Attribute);
				TestEqual(TEXT("Check payload OldValue"), Payload.OldValue, 0.f);
				TestEqual(TEXT("Check payload NewValue"), Payload.NewValue, 10.f);
			}
		});

		It(TEXT("UGBAAttributeSetBlueprintBase::PreAttributeBaseChange"), [this]()
		{
			TMap<FName, FGameplayAttribute> StorageKeys = GetStorageTestMap(TEXT("PreAttributeBaseChange"));

			// Check storage is empty before apply effect
			for (const TPair<FName, FGameplayAttribute>& Entry : StorageKeys)
			{
				FName StorageKey = Entry.Key;
				FGBATestStorage_PreAttributeBaseChangePayload Payload = GetStorage().GetValueAsPreAttributeBaseChangePayload(StorageKey);
				AddInfo(FString::Printf(TEXT("Checking storage for %s is invalid as expected"), *StorageKey.ToString()));
				TestFalse(FString::Printf(TEXT("Storage for %s is not initialized yet"), *StorageKey.ToString()), Payload.Attribute.IsValid());
			}

			// Apply effect, this should kick in BP events implemented in fixture BP AttributeSet
			ApplyGameplayEffect(TestASC, FixtureGameplayEffectLoadPath);

			// Check storage is filled after apply effect
			//
			// Fixture Attribute Set BP is implementing PostGameplayEffectExecute event and stores whatever it gets into the storage
			// so that we can check here it has been called, and with the proper parameters
			for (const TPair<FName, FGameplayAttribute>& Entry : StorageKeys)
			{
				FName StorageKey = Entry.Key;
				FGameplayAttribute Attribute = Entry.Value;
				FGBATestStorage_PreAttributeBaseChangePayload Payload = GetStorage().GetValueAsPreAttributeBaseChangePayload(StorageKey);
				AddInfo(FString::Printf(TEXT("Checking storage for %s was set as expected"), *StorageKey.ToString()));
				TestEqual(FString::Printf(TEXT("Storage for %s was initialized"), *StorageKey.ToString()), Payload.Attribute, Attribute);
				TestEqual(TEXT("Check payload Value"), Payload.Value, 10.f);
			}
		});


		It(TEXT("UGBAAttributeSetBlueprintBase::PostAttributeBaseChange"), [this]()
		{
			TMap<FName, FGameplayAttribute> StorageKeys = GetStorageTestMap(TEXT("PostAttributeBaseChange"));

			// Check storage is empty before apply effect
			for (const TPair<FName, FGameplayAttribute>& Entry : StorageKeys)
			{
				FName StorageKey = Entry.Key;
				FGBATestStorage_PostAttributeBaseChangePayload Payload = GetStorage().GetValueAsPostAttributeBaseChangePayload(StorageKey);
				AddInfo(FString::Printf(TEXT("Checking storage for %s is invalid as expected"), *StorageKey.ToString()));
				TestFalse(FString::Printf(TEXT("Storage for %s is not initialized yet"), *StorageKey.ToString()), Payload.Attribute.IsValid());
			}

			// Apply effect, this should kick in BP events implemented in fixture BP AttributeSet
			ApplyGameplayEffect(TestASC, FixtureGameplayEffectLoadPath);

			// Check storage is filled after apply effect
			//
			// Fixture Attribute Set BP is implementing PostGameplayEffectExecute event and stores whatever it gets into the storage
			// so that we can check here it has been called, and with the proper parameters
			for (const TPair<FName, FGameplayAttribute>& Entry : StorageKeys)
			{
				FName StorageKey = Entry.Key;
				FGameplayAttribute Attribute = Entry.Value;
				FGBATestStorage_PostAttributeBaseChangePayload Payload = GetStorage().GetValueAsPostAttributeBaseChangePayload(StorageKey);
				AddInfo(FString::Printf(TEXT("Checking storage for %s was set as expected"), *StorageKey.ToString()));
				TestEqual(FString::Printf(TEXT("Storage for %s was initialized"), *StorageKey.ToString()), Payload.Attribute, Attribute);
				TestEqual(TEXT("Check payload OldValue"), Payload.OldValue, 0.f);
				TestEqual(TEXT("Check payload NewValue"), Payload.NewValue, 10.f);
			}
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
