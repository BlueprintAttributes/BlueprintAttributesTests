// Copyright 2022-2024 Mickael Daniel. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "Abilities/GBAAttributeSetBlueprintBase.h"
#include "GBATestsStorageTypes.generated.h"

USTRUCT(BlueprintType)
struct FTestStorageBlueprintData
{
	GENERATED_BODY();

	FTestStorageBlueprintData() = default;

	virtual ~FTestStorageBlueprintData() = default;
};

USTRUCT(BlueprintType)
struct FGBATestStorage_PreGameplayEffectExecutePayload : public FTestStorageBlueprintData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Blueprint Attributes Tests")
	FGameplayAttribute Attribute;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Blueprint Attributes Tests")
	FGBAAttributeSetExecutionData ExecData;
};

USTRUCT(BlueprintType)
struct FGBATestStorage_PostGameplayEffectExecutePayload : public FTestStorageBlueprintData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Blueprint Attributes Tests")
	FGameplayAttribute Attribute;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Blueprint Attributes Tests")
	FGBAAttributeSetExecutionData ExecData;
};

USTRUCT(BlueprintType)
struct FGBATestStorage_PreAttributeChangePayload : public FTestStorageBlueprintData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Blueprint Attributes Tests")
	FGameplayAttribute Attribute;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Blueprint Attributes Tests")
	float Value = 0.f;
};

USTRUCT(BlueprintType)
struct FGBATestStorage_PostAttributeChangePayload : public FTestStorageBlueprintData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Blueprint Attributes Tests")
	FGameplayAttribute Attribute;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Blueprint Attributes Tests")
	float OldValue = 0.f;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Blueprint Attributes Tests")
	float NewValue = 0.f;
};

USTRUCT(BlueprintType)
struct FGBATestStorage_PreAttributeBaseChangePayload : public FTestStorageBlueprintData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Blueprint Attributes Tests")
	FGameplayAttribute Attribute;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Blueprint Attributes Tests")
	float Value = 0.f;
};

USTRUCT(BlueprintType)
struct FGBATestStorage_PostAttributeBaseChangePayload : public FTestStorageBlueprintData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Blueprint Attributes Tests")
	FGameplayAttribute Attribute;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Blueprint Attributes Tests")
	float OldValue = 0.f;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Blueprint Attributes Tests")
	float NewValue = 0.f;
};
