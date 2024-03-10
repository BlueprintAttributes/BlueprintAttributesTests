// Copyright 2022-2024 Mickael Daniel. All Rights Reserved.


#include "GBATestsStorageSubsystem.h"

#include "GBATestsLog.h"

void UGBATestsStorageSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	GBA_TESTS_LOG(Display, TEXT("UGBATestsStorageSubsystem::Initialize ..."))
}

void UGBATestsStorageSubsystem::Deinitialize()
{
	GBA_TESTS_LOG(Display, TEXT("UGBATestsStorageSubsystem::Deinitialize ..."))
	ResetStore();
	Super::Deinitialize();
}

void UGBATestsStorageSubsystem::ResetStore()
{
	StorePreGameplayEffectExecutePayload.Reset();
	StorePostGameplayEffectExecutePayload.Reset();
	StorePreAttributeChangePayload.Reset();
	StorePostAttributeChangePayload.Reset();
	StorePreAttributeBaseChangePayload.Reset();
	StorePostAttributeBaseChangePayload.Reset();
}

void UGBATestsStorageSubsystem::SetPreGameplayEffectExecutePayload(const FName Key, const FGBATestStorage_PreGameplayEffectExecutePayload& Payload)
{
	StorePreGameplayEffectExecutePayload.Add(Key, Payload);
}

FGBATestStorage_PreGameplayEffectExecutePayload UGBATestsStorageSubsystem::GetValueAsPreGameplayEffectExecutePayload(const FName Key) const
{
	const FGBATestStorage_PreGameplayEffectExecutePayload* Payload = StorePreGameplayEffectExecutePayload.Find(Key);
	if (!Payload)
	{
		// GBA_TESTS_LOG(Warning, TEXT("UGBATestsStorageSubsystem::GetValueAsPostGameplayEffectExecutePayload - Unable to get payload with key: %s"), *Identifier.ToString())
		return {};
	}

	return *Payload;
}

void UGBATestsStorageSubsystem::SetPostGameplayEffectExecutePayload(const FName Key, const FGBATestStorage_PostGameplayEffectExecutePayload& Payload)
{
	StorePostGameplayEffectExecutePayload.Add(Key, Payload);
}

FGBATestStorage_PostGameplayEffectExecutePayload UGBATestsStorageSubsystem::GetValueAsPostGameplayEffectExecutePayload(const FName Key) const
{
	const FGBATestStorage_PostGameplayEffectExecutePayload* Payload = StorePostGameplayEffectExecutePayload.Find(Key);
	if (!Payload)
	{
		// GBA_TESTS_LOG(Warning, TEXT("UGBATestsStorageSubsystem::GetValueAsPostGameplayEffectExecutePayload - Unable to get payload with key: %s"), *Identifier.ToString())
		return {};
	}

	return *Payload;
}

void UGBATestsStorageSubsystem::SetPreAttributeChangePayload(const FName Key, const FGBATestStorage_PreAttributeChangePayload& Payload)
{
	StorePreAttributeChangePayload.Add(Key, Payload);
}

FGBATestStorage_PreAttributeChangePayload UGBATestsStorageSubsystem::GetValueAsPreAttributeChangePayload(const FName Key) const
{
	const FGBATestStorage_PreAttributeChangePayload* Payload = StorePreAttributeChangePayload.Find(Key);
	if (!Payload)
	{
		// GBA_TESTS_LOG(Warning, TEXT("UGBATestsStorageSubsystem::GetValueAsPostGameplayEffectExecutePayload - Unable to get payload with key: %s"), *Identifier.ToString())
		return {};
	}

	return *Payload;
}

void UGBATestsStorageSubsystem::SetPreAttributeBaseChangePayload(const FName Key, const FGBATestStorage_PreAttributeBaseChangePayload& Payload)
{
	StorePreAttributeBaseChangePayload.Add(Key, Payload);
}

FGBATestStorage_PreAttributeBaseChangePayload UGBATestsStorageSubsystem::GetValueAsPreAttributeBaseChangePayload(const FName Key) const
{
	const FGBATestStorage_PreAttributeBaseChangePayload* Payload = StorePreAttributeBaseChangePayload.Find(Key);
	if (!Payload)
	{
		// GBA_TESTS_LOG(Warning, TEXT("UGBATestsStorageSubsystem::GetValueAsPostGameplayEffectExecutePayload - Unable to get payload with key: %s"), *Identifier.ToString())
		return {};
	}

	return *Payload;
}

void UGBATestsStorageSubsystem::SetPostAttributeBaseChangePayload(const FName Key, const FGBATestStorage_PostAttributeBaseChangePayload& Payload)
{
	StorePostAttributeBaseChangePayload.Add(Key, Payload);
}

FGBATestStorage_PostAttributeBaseChangePayload UGBATestsStorageSubsystem::GetValueAsPostAttributeBaseChangePayload(const FName Key) const
{
	const FGBATestStorage_PostAttributeBaseChangePayload* Payload = StorePostAttributeBaseChangePayload.Find(Key);
	if (!Payload)
	{
		// GBA_TESTS_LOG(Warning, TEXT("UGBATestsStorageSubsystem::GetValueAsPostGameplayEffectExecutePayload - Unable to get payload with key: %s"), *Identifier.ToString())
		return {};
	}

	return *Payload;
}

void UGBATestsStorageSubsystem::SetPostAttributeChangePayload(const FName Key, const FGBATestStorage_PostAttributeChangePayload& Payload)
{
	StorePostAttributeChangePayload.Add(Key, Payload);
}

FGBATestStorage_PostAttributeChangePayload UGBATestsStorageSubsystem::GetValueAsPostAttributeChangePayload(const FName Key) const
{
	const FGBATestStorage_PostAttributeChangePayload* Payload = StorePostAttributeChangePayload.Find(Key);
	if (!Payload)
	{
		// GBA_TESTS_LOG(Warning, TEXT("UGBATestsStorageSubsystem::GetValueAsPostGameplayEffectExecutePayload - Unable to get payload with key: %s"), *Identifier.ToString())
		return {};
	}

	return *Payload;
}
