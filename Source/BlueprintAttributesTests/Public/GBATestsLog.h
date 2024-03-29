﻿// Copyright 2022-2024 Mickael Daniel. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

BLUEPRINTATTRIBUTESTESTS_API DECLARE_LOG_CATEGORY_EXTERN(LogBlueprintAttributesTests, Display, All);

#define GBA_TESTS_LOG(Verbosity, Format, ...) \
{ \
    UE_LOG(LogBlueprintAttributesTests, Verbosity, Format, ##__VA_ARGS__); \
}
