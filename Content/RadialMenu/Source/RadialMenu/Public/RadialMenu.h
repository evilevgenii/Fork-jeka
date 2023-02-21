// Copyright 2020 Igor Lekic | All Rights Reserved | https://igorlekic.com/

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FRadialMenuModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
