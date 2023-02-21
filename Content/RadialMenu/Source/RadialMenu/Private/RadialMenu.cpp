// Copyright 2020 Igor Lekic | All Rights Reserved | https://igorlekic.com/

#include "RadialMenu.h"

#define LOCTEXT_NAMESPACE "FRadialMenuModule"

void FRadialMenuModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
}

void FRadialMenuModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FRadialMenuModule, RadialMenu)