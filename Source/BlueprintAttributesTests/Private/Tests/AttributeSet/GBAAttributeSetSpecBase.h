// Copyright 2022-2024 Mickael Daniel. All Rights Reserved.

#pragma once

#include "AbilitySystemComponent.h"
#include "AttributeSet.h"
#include "GBATestsStorageSubsystem.h"
#include "GameplayEffect.h"
#include "Misc/AutomationTest.h"

#define GBA_BEGIN_DEFINE_SPEC_WITH_BASE_PRIVATE( TClass, TBaseClass, PrettyName, TFlags, FileName, LineNumber ) \
	class TClass : public TBaseClass \
	{ \
	public: \
		TClass( const FString& InName ) \
		: TBaseClass( InName ) {\
			static_assert((TFlags)&EAutomationTestFlags::ApplicationContextMask, "AutomationTest has no application flag.  It shouldn't run.  See AutomationTest.h."); \
			static_assert(	(((TFlags)&EAutomationTestFlags::FilterMask) == EAutomationTestFlags::SmokeFilter) || \
							(((TFlags)&EAutomationTestFlags::FilterMask) == EAutomationTestFlags::EngineFilter) || \
							(((TFlags)&EAutomationTestFlags::FilterMask) == EAutomationTestFlags::ProductFilter) || \
							(((TFlags)&EAutomationTestFlags::FilterMask) == EAutomationTestFlags::PerfFilter) || \
							(((TFlags)&EAutomationTestFlags::FilterMask) == EAutomationTestFlags::StressFilter) || \
							(((TFlags)&EAutomationTestFlags::FilterMask) == EAutomationTestFlags::NegativeFilter), \
							"All AutomationTests must have exactly 1 filter type specified.  See AutomationTest.h."); \
		} \
		virtual uint32 GetTestFlags() const override { return TFlags; } \
		using FAutomationSpecBase::GetTestSourceFileName; \
		virtual FString GetTestSourceFileName() const override { return FileName; } \
		using FAutomationSpecBase::GetTestSourceFileLine; \
		virtual int32 GetTestSourceFileLine() const override { return LineNumber; } \
	protected: \
		virtual FString GetBeautifiedTestName() const override { return PrettyName; } \
		virtual void Define() override;

#if WITH_AUTOMATION_WORKER
	#define GBA_BEGIN_DEFINE_SPEC_WITH_BASE( TClass, TBaseClass, PrettyName, TFlags ) \
		GBA_BEGIN_DEFINE_SPEC_WITH_BASE_PRIVATE(TClass, TBaseClass, PrettyName, TFlags, __FILE__, __LINE__)

	#define GBA_END_DEFINE_SPEC( TClass ) \
		};\
		namespace\
		{\
			TClass TClass##AutomationSpecInstance( TEXT(#TClass) );\
		}
#endif

class UGBAAttributeSetBlueprintBase;

class FGBAAttributeSetSpecBase : public FAutomationSpecBase
{
public:
	FGBAAttributeSetSpecBase(const FString& InName) : FAutomationSpecBase(InName, false)
	{
	}

protected:

	UWorld* World = nullptr;
	uint64 InitialFrameCounter = 0;

	ACharacter* TestActor = nullptr;
	UAbilitySystemComponent* TestASC = nullptr;

	TSubclassOf<UAttributeSet> TestAttributeSetClass = nullptr;
	UGBAAttributeSetBlueprintBase* TestAttributeSet = nullptr;
	
	static constexpr const TCHAR* FixtureCharacterLoadPath = TEXT("/BlueprintAttributesTests/Fixtures/BP_Attributes_Test_Character.BP_Attributes_Test_Character_C");

	static UGBATestsStorageSubsystem& GetStorage()
	{
		return *GEngine->GetEngineSubsystem<UGBATestsStorageSubsystem>();
	}

	/** Returns a map with keys as expected storage keys, and value the expected attribute to be set with the payload */
	TMap<FName, FGameplayAttribute> GetStorageTestMap(const FString& InEventName) const
	{
		TArray<FName> Attributes = {
			TEXT("Vitality"),
			TEXT("Endurance"),
			TEXT("Strength"),
			TEXT("Dexterity"),
			TEXT("Intelligence"),
			TEXT("Faith"),
			TEXT("Luck")
		};

		TMap<FName, FGameplayAttribute> StorageKeys;
		for (const FName& Attribute : Attributes)
		{
			FString Key = FString::Printf(TEXT("%s_%s"), *InEventName, *Attribute.ToString());
			StorageKeys.Add(FName(*Key), GetAttributeProperty(TestAttributeSetClass, Attribute));
		}

		return StorageKeys;
	}

	bool ApplyGameplayEffect(UAbilitySystemComponent* ASC, const FString& EffectLoadPath, const float Level = 1.f)
	{
		const TSubclassOf<UGameplayEffect> Effect = StaticLoadClass(UGameplayEffect::StaticClass(), nullptr, *EffectLoadPath);
		if (!IsValid(Effect))
		{
			AddError(FString::Printf(TEXT("Unable to load %s"), *EffectLoadPath));
			return false;
		}

		AddInfo(FString::Printf(TEXT("Applying effect %s (%s)"), *Effect->GetName(), *EffectLoadPath));
		const FActiveGameplayEffectHandle Handle = ASC->BP_ApplyGameplayEffectToSelf(Effect, Level, ASC->MakeEffectContext());
		return Handle.IsValid();
	}

	static UDataTable* StaticLoadDataTable(const FString& InPackageName)
	{
		return Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, *InPackageName));
	}

	static UWorld* CreateWorld(uint64& OutInitialFrameCounter)
	{
		// Setup tests
		UWorld* World = UWorld::CreateWorld(EWorldType::Game, false);
		check(World);

		FWorldContext& WorldContext = GEngine->CreateNewWorldContext(EWorldType::Game);
		WorldContext.SetCurrentWorld(World);

		const FURL URL;
		World->InitializeActorsForPlay(URL);
		World->BeginPlay();

		// Tick a small number to verify the application tick
		TickWorld(World, SMALL_NUMBER);

		TIndirectArray<FWorldContext> WorldContexts = GEngine->GetWorldContexts();

		OutInitialFrameCounter = GFrameCounter;

		return World;
	}

	static void TickWorld(UWorld* InWorld, float Time)
	{
		while (Time > 0.f)
		{
			constexpr float Step = 0.1f;
			InWorld->Tick(LEVELTICK_All, FMath::Min(Time, Step));
			Time -= Step;

			// This is terrible but required for sub ticking like this.
			// we could always cache the real GFrameCounter at the start of our tests and restore it when finished.
			GFrameCounter++;
		}
	}

	static void TeardownWorld(UWorld* InWorld, const uint64 InFrameCounter)
	{
		GFrameCounter = InFrameCounter;

		// Teardown tests
		GEngine->DestroyWorldContext(InWorld);
		InWorld->DestroyWorld(false);
	}

	static FGameplayAttribute GetAttributeProperty(const UClass* InClass, const FName& InPropertyName)
	{
		return FindFProperty<FProperty>(InClass, InPropertyName);
	}

	static bool HasAttributeSet(const UAbilitySystemComponent* ASC, const TSubclassOf<UAttributeSet> AttributeClass)
	{
		if (!ASC)
		{
			return false;
		}

		TArray<UAttributeSet*> AttributeSets = ASC->GetSpawnedAttributes();
		for (const UAttributeSet* Set : AttributeSets)
		{
			// if (Set && Set->IsA(AttributeClass))

			// We want to test for strict class equality (not child)
			if (Set && Set->GetClass() == AttributeClass)
			{
				return true;
			}
		}

		return false;
	}

	void TestAttribute(const FName& InAttributePropertyName, const UClass* InAttributesSetClass = nullptr)
	{
		AddInfo(FString::Printf(TEXT("Checking Attribute %s"), *InAttributePropertyName.ToString()));
		const UClass* AttributeSetClass = InAttributesSetClass ? InAttributesSetClass : TestAttributeSetClass;

		const FGameplayAttribute Attribute = GetAttributeProperty(AttributeSetClass, InAttributePropertyName);
		if (!Attribute.IsValid())
		{
			AddError(FString::Printf(TEXT("Attribute %s is not valid (is it granted ?)"), *InAttributePropertyName.ToString()));
			return;
		}

		TestTrue(FString::Printf(TEXT("Attribute %s is valid"), *InAttributePropertyName.ToString()), Attribute.IsValid());
		TestEqual(
			FString::Printf(TEXT("Attribute %s value is valid"), *InAttributePropertyName.ToString()),
			TestASC->GetNumericAttribute(Attribute),
			0.f
		);
	}

	void TestAttribute(const FName& InAttributePropertyName, const float InExpectedValue, const UClass* InAttributesSetClass = nullptr)
	{
		AddInfo(FString::Printf(TEXT("Checking Attribute %s for equality with %f"), *InAttributePropertyName.ToString(), InExpectedValue));

		const UClass* AttributeSetClass = InAttributesSetClass ? InAttributesSetClass : TestAttributeSetClass;

		const FGameplayAttribute Attribute = GetAttributeProperty(AttributeSetClass, InAttributePropertyName);
		if (!Attribute.IsValid())
		{
			AddError(FString::Printf(TEXT("Attribute %s is not valid (is it granted ?)"), *InAttributePropertyName.ToString()));
			return;
		}

		TestTrue(FString::Printf(TEXT("Attribute %s is valid"), *InAttributePropertyName.ToString()), Attribute.IsValid());
		TestEqual(
			FString::Printf(TEXT("Attribute %s value is %f"), *InAttributePropertyName.ToString(), InExpectedValue),
			TestASC->GetNumericAttribute(Attribute),
			InExpectedValue
		);
	};

	static FGameplayModifierInfo& AddModifier(UGameplayEffect* Effect, FProperty* Property, EGameplayModOp::Type Op, const FScalableFloat& Magnitude)
	{
		const int32 Idx = Effect->Modifiers.Num();
		Effect->Modifiers.SetNum(Idx + 1);
		FGameplayModifierInfo& Info = Effect->Modifiers[Idx];
		Info.ModifierMagnitude = Magnitude;
		Info.ModifierOp = Op;
		Info.Attribute.SetUProperty(Property);
		return Info;
	}
	
	struct FAttributeTestDefinition
	{
		FName AttributeName;
		float InitialValue;

		FAttributeTestDefinition() = default;

		FAttributeTestDefinition(const FName& InAttributeName, const float InInitialValue)
			: AttributeName(InAttributeName),
			  InitialValue(InInitialValue)
		{
		}

		bool IsValid() const
		{
			return !AttributeName.IsNone();
		}
	};

	float GetClampedExpectedValue(const FAttributeTestDefinition& InMinAttributeDef, const FAttributeTestDefinition& InMaxAttributeDef, const float InValue) const
	{
		float ExpectedValue = -9999.f;
		if (InMinAttributeDef.IsValid() && InMaxAttributeDef.IsValid())
		{
			const FGameplayAttribute MinAttribute = GetAttributeProperty(TestAttributeSetClass, InMinAttributeDef.AttributeName);
			const FGameplayAttribute MaxAttribute = GetAttributeProperty(TestAttributeSetClass, InMaxAttributeDef.AttributeName);
			ExpectedValue = FMath::Clamp(InValue, TestASC->GetNumericAttribute(MinAttribute), TestASC->GetNumericAttribute(MaxAttribute));
		}
		else if (InMinAttributeDef.IsValid())
		{
			const FGameplayAttribute MinAttribute = GetAttributeProperty(TestAttributeSetClass, InMinAttributeDef.AttributeName);
			ExpectedValue = FMath::Max(InValue, TestASC->GetNumericAttribute(MinAttribute));
		}
		else if (InMaxAttributeDef.IsValid())
		{
			const FGameplayAttribute MaxAttribute = GetAttributeProperty(TestAttributeSetClass, InMaxAttributeDef.AttributeName);
			ExpectedValue = FMath::Min(InValue, TestASC->GetNumericAttribute(MaxAttribute));
		}

		return ExpectedValue;
	}

	void TestAttributeClamping(const FAttributeTestDefinition& InAttributeDef, const FAttributeTestDefinition& InMinAttributeDef, const FAttributeTestDefinition& InMaxAttributeDef)
	{
		const FName AttributeName = InAttributeDef.AttributeName;
		const float AttributeInitialValue = InAttributeDef.InitialValue;

		TestAttribute(AttributeName, AttributeInitialValue);
		
		if (InMinAttributeDef.IsValid())
		{
			TestAttribute(InMinAttributeDef.AttributeName, InMinAttributeDef.InitialValue);
		}
		
		if (InMaxAttributeDef.IsValid())
		{
			TestAttribute(InMaxAttributeDef.AttributeName, InMaxAttributeDef.InitialValue);
		}

		const FGameplayAttribute Attribute = GetAttributeProperty(TestAttributeSetClass, AttributeName);

		constexpr float DamageValue = 100.f;

		// Effect has a -100 modifier for Stamina
		// just try and reduce the health attribute
		{
			const FString EffectName = FString::Printf(TEXT("%sDamageEffect"), *AttributeName.ToString());
			UGameplayEffect* DamageEffect = NewObject<UGameplayEffect>(GetTransientPackage(), FName(*EffectName));

			FProperty* Property = FindFieldChecked<FProperty>(TestAttributeSetClass, AttributeName);
			AddModifier(DamageEffect, Property, EGameplayModOp::Additive, FScalableFloat(-DamageValue));

			DamageEffect->DurationPolicy = EGameplayEffectDurationType::Instant;
			
			TestASC->ApplyGameplayEffectToSelf(DamageEffect, 1.f, FGameplayEffectContextHandle());

			float ExpectedValue = GetClampedExpectedValue(InMinAttributeDef, InMaxAttributeDef, AttributeInitialValue - DamageValue);
			TestAttribute(AttributeName, ExpectedValue);

			TestASC->ApplyGameplayEffectToSelf(DamageEffect, 1.f, FGameplayEffectContextHandle());
			
			ExpectedValue = GetClampedExpectedValue(InMinAttributeDef, InMaxAttributeDef, AttributeInitialValue - 2 * DamageValue);
			TestAttribute(AttributeName, ExpectedValue);
		}

		// Reset to initial value
		{
			const FString EffectName = FString::Printf(TEXT("%sResetEffect"), *AttributeName.ToString());
			UGameplayEffect* OverrideEffect = NewObject<UGameplayEffect>(GetTransientPackage(), FName(*EffectName));

			FProperty* Property = FindFieldChecked<FProperty>(TestAttributeSetClass, AttributeName);
			AddModifier(OverrideEffect, Property, EGameplayModOp::Override, AttributeInitialValue);

			OverrideEffect->DurationPolicy = EGameplayEffectDurationType::Instant;
			TestASC->ApplyGameplayEffectToSelf(OverrideEffect, 1.f, FGameplayEffectContextHandle());

			const float ExpectedValue = GetClampedExpectedValue(InMinAttributeDef, InMaxAttributeDef, AttributeInitialValue);
			TestAttribute(AttributeName, ExpectedValue);
		}

		constexpr int32 NumPeriods = 10;
		constexpr float PeriodSecs = 1.0f;
		constexpr float MagnitudePerPeriod = 5.f;
		const float StartingAttributeValue = TestASC->GetNumericAttribute(Attribute);

		// Effect is an infinite, regen effect adding a set amount of Stamina every 1 seconds
		// just try and regen the Stamina attribute
		{
			const FString EffectName = FString::Printf(TEXT("%sRegenEffect"), *AttributeName.ToString());
			UGameplayEffect* BaseRegenEffect = NewObject<UGameplayEffect>(GetTransientPackage(), FName(*EffectName));

			FProperty* Property = FindFieldChecked<FProperty>(TestAttributeSetClass, AttributeName);
			AddModifier(BaseRegenEffect, Property, EGameplayModOp::Additive, FScalableFloat(MagnitudePerPeriod));

			BaseRegenEffect->DurationPolicy = EGameplayEffectDurationType::Infinite;
			BaseRegenEffect->Period.Value = PeriodSecs;

			TestASC->ApplyGameplayEffectToSelf(BaseRegenEffect, 1.f, FGameplayEffectContextHandle());
		}

		int32 NumApplications = 0;

		// Tick a small number to verify the application tick
		TickWorld(World, SMALL_NUMBER);
		++NumApplications;

		TestAttribute(AttributeName, StartingAttributeValue + (MagnitudePerPeriod * NumApplications));

		// Tick a bit more to address possible floating point issues
		TickWorld(World, PeriodSecs * .1f);

		// Tick for 4 times as long
		for (int32 i = 0; i < NumPeriods * 4; ++i)
		{
			// advance time by one period
			TickWorld(World, PeriodSecs);

			++NumApplications;

			// check that Stamina has been increased, but not indefinitely. Should be clamped up to the value of MaxStamina
			AddInfo(FString::Printf(
				TEXT("Value after tick Current: %f (Base: %f) - %s"),
				TestASC->GetNumericAttribute(Attribute),
				TestASC->GetNumericAttributeBase(Attribute),
				*Attribute.GetName()
			));

			const float ExpectedValue = GetClampedExpectedValue(InMinAttributeDef, InMaxAttributeDef, StartingAttributeValue + MagnitudePerPeriod * NumApplications);
			TestAttribute(AttributeName, ExpectedValue);
		}
	}

	void DescribeAttributeClamp(const FString& InDescription, const FString& InFixtureAssetName, TFunction<void()> DoWork)
	{
		const FString FixtureLoadPath = FString::Printf(
			TEXT("/BlueprintAttributesTests/Fixtures/AttributeBasedClamping/%s.%s_C"),
			*InFixtureAssetName,
			*InFixtureAssetName
		);
		
		Describe(InDescription, [this, FixtureLoadPath, DoWork]()
		{
			BeforeEach([this, FixtureLoadPath]()
			{
				// Grab fixture Attribute Set class for further use later on
				TestAttributeSetClass = StaticLoadClass(UAttributeSet::StaticClass(), nullptr, *FixtureLoadPath);
				if (!IsValid(TestAttributeSetClass))
				{
					AddError(FString::Printf(TEXT("Unable to load %s"), *FixtureLoadPath));
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
		
			It(TEXT("should have attributes clamped after Gameplay Effect application"), DoWork);
		});
	}
};
