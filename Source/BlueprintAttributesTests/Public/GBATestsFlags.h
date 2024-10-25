﻿// Copyright 2022-2024 Mickael Daniel. All Rights Reserved.

#pragma once

#include "Misc/EngineVersionComparison.h"

#if UE_VERSION_OLDER_THAN(5, 5, 0)

// 5.4.x and down
inline constexpr uint8 EAutomationTestFlags_ApplicationContextMask = EAutomationTestFlags::EditorContext | EAutomationTestFlags::ClientContext | EAutomationTestFlags::ServerContext | EAutomationTestFlags::CommandletContext;

#endif
