// Copyright 2021-2022 Mickael Daniel. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameplayTagsManager.h"

// ReSharper disable once CppPolymorphicClassWithNonVirtualPublicDestructor
struct BLUEPRINTATTRIBUTESTESTS_API FGBATestsNativeTags : public FGameplayTagNativeAdder
{
	FGameplayTag TestEffect;

	FORCEINLINE static const FGBATestsNativeTags& Get() { return NativeTags; }

	virtual void AddTags() override
	{
		UGameplayTagsManager& Manager = UGameplayTagsManager::Get();

		TestEffect = Manager.AddNativeGameplayTag(TEXT("GameplayEffect.Test.BlueprintOverride"));
	}

private:
	static FGBATestsNativeTags NativeTags;
};
