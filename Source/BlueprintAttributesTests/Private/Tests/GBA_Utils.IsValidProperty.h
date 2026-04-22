// Copyright 2022-2026 Mickael Daniel. All Rights Reserved.

#pragma once

#include "Abilities/GBAAttributeSetBlueprintBase.h"
#include "AttributeSet.h"
#include "GBA_Utils.IsValidProperty.generated.h"

USTRUCT()
struct FGBAGameplayAttributeDataTest : public FGameplayAttributeData
{
	GENERATED_BODY()

	FGBAGameplayAttributeDataTest() = default;

	FGBAGameplayAttributeDataTest(const float DefaultValue)
		: FGameplayAttributeData(DefaultValue)
	{
	}
};

UCLASS(meta=(HideInDetailsView))
class UGBATestAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	UPROPERTY()
	FGameplayAttributeData Test = 0.f;

	UPROPERTY()
	FGBAGameplayClampedAttributeData TestClamped = 0.f;

	UPROPERTY()
	FGBAGameplayAttributeDataTest TestCustom = 0.f;
};

UCLASS()
class UGBATestDummy : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY()
	FGameplayTag TestTag;

	UPROPERTY()
	FGameplayAttribute TestAttribute;
};
