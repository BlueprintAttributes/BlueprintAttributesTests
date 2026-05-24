// Copyright 2022-2026 Mickael Daniel. All Rights Reserved.

#include "AttributeSet.h"
#include "GameplayEffectExecutionCalculation.h"
#include "GameplayEffectTypes.h"
#include "Misc/AutomationTest.h"
#include "Utils/GBAExecutionCalculationBlueprintLibrary.h"

BEGIN_DEFINE_SPEC(FGBAExecutionCalculationSpec, "BlueprintAttributes.GBAExecutionCalculation", EAutomationTestFlags::ProductFilter | EAutomationTestFlags_ApplicationContextMask)

	const FString FixtureAttributeSetLoadPath = TEXT("/BlueprintAttributesTests/Fixtures/GBAAttributeSetBlueprintBase_Spec/GBA_Test_Stats.GBA_Test_Stats_C");
	TSubclassOf<UAttributeSet> TestAttributeSetClass = nullptr;

	static FGameplayAttribute GetAttributeProperty(const UClass* InClass, const FName& InPropertyName)
	{
		return FindFProperty<FProperty>(InClass, InPropertyName);
	}

END_DEFINE_SPEC(FGBAExecutionCalculationSpec)

void FGBAExecutionCalculationSpec::Define()
{
	Describe(TEXT("GBAExecutionCalculationBlueprintLibrary::FindCaptureDefinition()"), [this]()
	{
		BeforeEach([this]()
		{
			TestAttributeSetClass = StaticLoadClass(UAttributeSet::StaticClass(), nullptr, *FixtureAttributeSetLoadPath);
			if (!IsValid(TestAttributeSetClass))
			{
				AddError(FString::Printf(TEXT("Unable to load %s"), *FixtureAttributeSetLoadPath));
			}
		});

		It(TEXT("finds matching capture definition in array"), [this]()
		{
			const FGameplayAttribute Attribute = GetAttributeProperty(TestAttributeSetClass, TEXT("Strength"));
			if (!Attribute.IsValid())
			{
				AddError(TEXT("Strength attribute is not valid"));
				return;
			}

			TArray<FGameplayEffectAttributeCaptureDefinition> Captures;
			Captures.Add(FGameplayEffectAttributeCaptureDefinition(Attribute, EGameplayEffectAttributeCaptureSource::Source, false));
			Captures.Add(FGameplayEffectAttributeCaptureDefinition(Attribute, EGameplayEffectAttributeCaptureSource::Target, true));

			FGameplayEffectAttributeCaptureDefinition OutDef;
			const bool Found = UGBAExecutionCalculationBlueprintLibrary::FindCaptureDefinition(
				Captures,
				Attribute,
				EGameplayEffectAttributeCaptureSource::Source,
				false,
				OutDef
			);

			TestTrue(TEXT("Found matching definition"), Found);
			TestTrue(TEXT("Output definition attribute matches"), OutDef.AttributeToCapture == Attribute);
			TestEqual(TEXT("Output definition source is Source"), OutDef.AttributeSource, EGameplayEffectAttributeCaptureSource::Source);
			TestFalse(TEXT("Output definition bSnapshot is false"), OutDef.bSnapshot);
		});

		It(TEXT("returns false for non-matching attribute"), [this]()
		{
			const FGameplayAttribute Attribute = GetAttributeProperty(TestAttributeSetClass, TEXT("Strength"));
			const FGameplayAttribute OtherAttribute = GetAttributeProperty(TestAttributeSetClass, TEXT("Dexterity"));

			if (!Attribute.IsValid() || !OtherAttribute.IsValid())
			{
				AddError(TEXT("Attributes are not valid"));
				return;
			}

			TArray<FGameplayEffectAttributeCaptureDefinition> Captures;
			Captures.Add(FGameplayEffectAttributeCaptureDefinition(OtherAttribute, EGameplayEffectAttributeCaptureSource::Source, false));

			FGameplayEffectAttributeCaptureDefinition OutDef;
			const bool Found = UGBAExecutionCalculationBlueprintLibrary::FindCaptureDefinition(
				Captures,
				Attribute,
				EGameplayEffectAttributeCaptureSource::Source,
				false,
				OutDef
			);

			TestFalse(TEXT("Did not find non-matching attribute"), Found);
		});

		It(TEXT("returns false for empty array"), [this]()
		{
			const FGameplayAttribute Attribute = GetAttributeProperty(TestAttributeSetClass, TEXT("Strength"));
			TArray<FGameplayEffectAttributeCaptureDefinition> EmptyCaptures;
			FGameplayEffectAttributeCaptureDefinition OutDef;

			const bool Found = UGBAExecutionCalculationBlueprintLibrary::FindCaptureDefinition(
				EmptyCaptures, Attribute, EGameplayEffectAttributeCaptureSource::Source, false, OutDef
			);

			TestFalse(TEXT("Did not find in empty array"), Found);
		});

		It(TEXT("distinguishes between Source and Target capture source"), [this]()
		{
			const FGameplayAttribute Attribute = GetAttributeProperty(TestAttributeSetClass, TEXT("Strength"));
			if (!Attribute.IsValid())
			{
				AddError(TEXT("Strength attribute is not valid"));
				return;
			}

			TArray<FGameplayEffectAttributeCaptureDefinition> Captures;
			Captures.Add(FGameplayEffectAttributeCaptureDefinition(Attribute, EGameplayEffectAttributeCaptureSource::Target, false));

			FGameplayEffectAttributeCaptureDefinition OutDef;
			const bool Found = UGBAExecutionCalculationBlueprintLibrary::FindCaptureDefinition(
				Captures, Attribute, EGameplayEffectAttributeCaptureSource::Source, false, OutDef
			);

			TestFalse(TEXT("Did not match Source when only Target exists"), Found);
		});

		It(TEXT("distinguishes between snapshot and non-snapshot"), [this]()
		{
			const FGameplayAttribute Attribute = GetAttributeProperty(TestAttributeSetClass, TEXT("Strength"));
			if (!Attribute.IsValid())
			{
				AddError(TEXT("Strength attribute is not valid"));
				return;
			}

			TArray<FGameplayEffectAttributeCaptureDefinition> Captures;
			Captures.Add(FGameplayEffectAttributeCaptureDefinition(Attribute, EGameplayEffectAttributeCaptureSource::Source, true));

			FGameplayEffectAttributeCaptureDefinition OutDef;
			const bool FoundSnapshot = UGBAExecutionCalculationBlueprintLibrary::FindCaptureDefinition(
				Captures,
				Attribute,
				EGameplayEffectAttributeCaptureSource::Source,
				true,
				OutDef
			);

			const bool FoundNonSnapshot = UGBAExecutionCalculationBlueprintLibrary::FindCaptureDefinition(
				Captures,
				Attribute,
				EGameplayEffectAttributeCaptureSource::Source,
				false,
				OutDef
			);

			TestTrue(TEXT("Found snapshot definition"), FoundSnapshot);
			TestFalse(TEXT("Did not find non-snapshot when only snapshot exists"), FoundNonSnapshot);
		});
	});

	Describe(TEXT("GBAExecutionCalculationBlueprintLibrary::AddOutputModifier()"), [this]()
	{
		BeforeEach([this]()
		{
			TestAttributeSetClass = StaticLoadClass(UAttributeSet::StaticClass(), nullptr, *FixtureAttributeSetLoadPath);
			if (!IsValid(TestAttributeSetClass))
			{
				AddError(FString::Printf(TEXT("Unable to load %s"), *FixtureAttributeSetLoadPath));
			}
		});

		It(TEXT("adds modifier and returns reference to the same output"), [this]()
		{
			const FGameplayAttribute Attribute = GetAttributeProperty(TestAttributeSetClass, TEXT("Strength"));
			if (!Attribute.IsValid())
			{
				AddError(TEXT("Strength attribute is not valid"));
				return;
			}

			FGameplayEffectCustomExecutionOutput Output;
			const FGameplayEffectCustomExecutionOutput& Result = UGBAExecutionCalculationBlueprintLibrary::AddOutputModifier(
				Output,
				Attribute,
				EGameplayModOp::Additive,
				42.0f
			);

			TestTrue(TEXT("Returned reference is the same object"), &Result == &Output);
			TestFalse(TEXT("Output has at least one modifier"), Output.GetOutputModifiers().IsEmpty());
		});

		It(TEXT("adds correct modifier data"), [this]()
		{
			const FGameplayAttribute Attribute = GetAttributeProperty(TestAttributeSetClass, TEXT("Strength"));
			if (!Attribute.IsValid())
			{
				AddError(TEXT("Strength attribute is not valid"));
				return;
			}

			constexpr float TestMagnitude = 123.4f;

			constexpr EGameplayModOp::Type ModOperation = EGameplayModOp::MultiplyAdditive;

			FGameplayEffectCustomExecutionOutput Output;
			UGBAExecutionCalculationBlueprintLibrary::AddOutputModifier(
				Output,
				Attribute,
				ModOperation,
				TestMagnitude
			);

			const FGameplayModifierEvaluatedData& Modifier = Output.GetOutputModifiers()[0];
			TestTrue(TEXT("Modifier attribute matches"), Modifier.Attribute == Attribute);
			TestEqual(TEXT("Modifier operation is Multiply"), Modifier.ModifierOp, ModOperation);
			TestEqual(TEXT("Modifier magnitude matches"), Modifier.Magnitude, TestMagnitude);
		});
	});

	Describe(TEXT("GBAExecutionCalculationBlueprintLibrary::MarkStackCountHandledManually()"), [this]()
	{
		It(TEXT("returns reference to the same execution output"), [this]()
		{
			FGameplayEffectCustomExecutionOutput Output;
			const FGameplayEffectCustomExecutionOutput& Result = UGBAExecutionCalculationBlueprintLibrary::MarkStackCountHandledManually(Output);
			TestTrue(TEXT("Returned reference is the same object"), &Result == &Output);
		});
	});

	Describe(TEXT("GBAExecutionCalculationBlueprintLibrary::MarkConditionalGameplayEffectsToTrigger()"), [this]()
	{
		It(TEXT("returns reference to the same execution output"), [this]()
		{
			FGameplayEffectCustomExecutionOutput Output;
			const FGameplayEffectCustomExecutionOutput& Result = UGBAExecutionCalculationBlueprintLibrary::MarkConditionalGameplayEffectsToTrigger(Output);
			TestTrue(TEXT("Returned reference is the same object"), &Result == &Output);
		});
	});

	Describe(TEXT("GBAExecutionCalculationBlueprintLibrary::MarkGameplayCuesHandledManually()"), [this]()
	{
		It(TEXT("returns reference to the same execution output"), [this]()
		{
			FGameplayEffectCustomExecutionOutput Output;
			const FGameplayEffectCustomExecutionOutput& Result = UGBAExecutionCalculationBlueprintLibrary::MarkGameplayCuesHandledManually(Output);
			TestTrue(TEXT("Returned reference is the same object"), &Result == &Output);
		});
	});
}
