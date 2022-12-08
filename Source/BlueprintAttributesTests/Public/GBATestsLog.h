// Copyright 2021-2022 Mickael Daniel. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

BLUEPRINTATTRIBUTESTESTS_API DECLARE_LOG_CATEGORY_EXTERN(LogGameplayBlueprintAttributesTests, Display, All);

#define GBA_TESTS_LOG(Verbosity, Format, ...) \
{ \
    UE_LOG(LogGameplayBlueprintAttributesTests, Verbosity, Format, ##__VA_ARGS__); \
}
