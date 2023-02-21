// Copyright 2020 Igor Lekic | All Rights Reserved | https://igorlekic.com/

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "DzX_RadialMenu_Interface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UDzX_RadialMenu_Interface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class RADIALMENU_API IDzX_RadialMenu_Interface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Radial Menu Call")
		void RadialButtonClick(class UDzX_RadialMenu* RadialMenu);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Radial Menu Call")
		void RadialMenuHoverOver(class UDzX_RadialMenu* RadialMenu);
};
