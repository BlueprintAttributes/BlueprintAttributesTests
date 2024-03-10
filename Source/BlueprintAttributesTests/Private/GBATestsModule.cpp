// Copyright 2022-2024 Mickael Daniel. All Rights Reserved.

#include "GBATestsModule.h"

#define LOCTEXT_NAMESPACE "FGBATestsModule"

void FGBATestsModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
}

void FGBATestsModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FGBATestsModule, BlueprintAttributesTests)