// Copyright 2020 Igor Lekic | All Rights Reserved | https://igorlekic.com/
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "DzX_RadialMenu_Button.generated.h"

UENUM(BlueprintType)
enum class EButtonState : uint8
{
	Active				UMETA(DisplayName = "Currently active"),
	Normal				UMETA(DisplayName = "Available but not active"),
	Disabled			UMETA(DisplayName = "Not available"),
	HoveredNormal		UMETA(DisplayName = "Hovered - normal"),
	HoveredDisabled		UMETA(DisplayName = "Hovered - disabled"),

};
/**
 * 
 */
UCLASS()
class RADIALMENU_API UDzX_RadialMenu_Button : public UUserWidget
{
	GENERATED_BODY()
#pragma region Variables
public:
	//Available but not selected,hovered or toggled active.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Button look")
		FSlateBrush Normal;
	//Not available
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Button look")
		FSlateBrush Disabled;
	//Button is toggled activate.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Button look")
		FSlateBrush Active;
	//Button is available and hovered.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Button look")
		FSlateBrush HoveredNormal;
	//Button is NOT available and hovered.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Button look")
		FSlateBrush HoveredDisabled;
	//Current state of this button
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category = "Button look")
		EButtonState CurrentButtonState = EButtonState::Normal;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Button look", meta = (BindWidget))
		class UImage* ButtonLook;

public:
	//Button position in radial menu (ID ex. 0,1,2,3)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Params")
		int32 ButtonPosition = -1;
#pragma endregion Variables

#pragma region Functions
protected:
	virtual bool Initialize() override;
public:
	//Change look of current button
	UFUNCTION(BlueprintCallable, Category = "Functions")
		void ChangeCurrentState(EButtonState NewState);
	//Refresh button look
	UFUNCTION(BlueprintCallable, Category = "Functions")
		void Refresh();
	//Button clicked event
	UFUNCTION(Category = "Functions")
		virtual void ClickEvent();
	//Button clicked blueprint event
	UFUNCTION(BlueprintNativeEvent, Category = "Functions")
		void OnClicked();

#pragma endregion Functions

};
