// Copyright 2022-2024 Mickael Daniel. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GBATestsStorageTypes.h"
#include "Subsystems/EngineSubsystem.h"
#include "GBATestsStorageSubsystem.generated.h"

/**
 * Test oriented world subsystem to store arbitrary value, and be able to check execution of
 * things and assert expected values.
 */
UCLASS(DisplayName = "Tests Storage")
class BLUEPRINTATTRIBUTESTESTS_API UGBATestsStorageSubsystem : public UEngineSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	// TODO: Learn how to setup wildcards

	// UFUNCTION(BlueprintCallable, CustomThunk, Category="Blueprint Attributes Tests", meta = (CustomStructureParam = "Value", AutoCreateRefTerm = "Value"))
	// static void SetTestStoragePayload(const FString& FieldName, const int32& Payload);
	// DECLARE_FUNCTION(SetTestStoragePayload);

	UFUNCTION(BlueprintCallable, Category="Blueprint Attributes Tests")
	void ResetStore();

	UFUNCTION(BlueprintCallable, Category="Blueprint Attributes Tests")
	void SetPreGameplayEffectExecutePayload(const FName Key, const FGBATestStorage_PreGameplayEffectExecutePayload& Payload);

	UFUNCTION(BlueprintPure, Category="Blueprint Attributes Tests")
	FGBATestStorage_PreGameplayEffectExecutePayload GetValueAsPreGameplayEffectExecutePayload(const FName Key) const;

	UFUNCTION(BlueprintCallable, Category="Blueprint Attributes Tests")
	void SetPostGameplayEffectExecutePayload(const FName Key, const FGBATestStorage_PostGameplayEffectExecutePayload& Payload);

	UFUNCTION(BlueprintPure, Category="Blueprint Attributes Tests")
	FGBATestStorage_PostGameplayEffectExecutePayload GetValueAsPostGameplayEffectExecutePayload(const FName Key) const;

	UFUNCTION(BlueprintCallable, Category="Blueprint Attributes Tests")
	void SetPreAttributeChangePayload(const FName Key, const FGBATestStorage_PreAttributeChangePayload& Payload);

	UFUNCTION(BlueprintPure, Category="Blueprint Attributes Tests")
	FGBATestStorage_PreAttributeChangePayload GetValueAsPreAttributeChangePayload(const FName Key) const;

	UFUNCTION(BlueprintCallable, Category="Blueprint Attributes Tests")
	void SetPostAttributeChangePayload(const FName Key, const FGBATestStorage_PostAttributeChangePayload& Payload);

	UFUNCTION(BlueprintPure, Category="Blueprint Attributes Tests")
	FGBATestStorage_PostAttributeChangePayload GetValueAsPostAttributeChangePayload(const FName Key) const;
	
	UFUNCTION(BlueprintCallable, Category="Blueprint Attributes Tests")
	void SetPreAttributeBaseChangePayload(const FName Key, const FGBATestStorage_PreAttributeBaseChangePayload& Payload);

	UFUNCTION(BlueprintPure, Category="Blueprint Attributes Tests")
	FGBATestStorage_PreAttributeBaseChangePayload GetValueAsPreAttributeBaseChangePayload(const FName Key) const;

	UFUNCTION(BlueprintCallable, Category="Blueprint Attributes Tests")
	void SetPostAttributeBaseChangePayload(const FName Key, const FGBATestStorage_PostAttributeBaseChangePayload& Payload);

	UFUNCTION(BlueprintPure, Category="Blueprint Attributes Tests")
	FGBATestStorage_PostAttributeBaseChangePayload GetValueAsPostAttributeBaseChangePayload(const FName Key) const;

private:
	// TODO: Store only FTestStorageBlueprintData types (but has to resort to static const_cast shenanigans)
	TMap<FName, FGBATestStorage_PreGameplayEffectExecutePayload> StorePreGameplayEffectExecutePayload;
	TMap<FName, FGBATestStorage_PostGameplayEffectExecutePayload> StorePostGameplayEffectExecutePayload;
	TMap<FName, FGBATestStorage_PreAttributeChangePayload> StorePreAttributeChangePayload;
	TMap<FName, FGBATestStorage_PostAttributeChangePayload> StorePostAttributeChangePayload;
	TMap<FName, FGBATestStorage_PreAttributeBaseChangePayload> StorePreAttributeBaseChangePayload;
	TMap<FName, FGBATestStorage_PostAttributeBaseChangePayload> StorePostAttributeBaseChangePayload;
};
