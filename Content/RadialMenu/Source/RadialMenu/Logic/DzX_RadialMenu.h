// Copyright 2020 Igor Lekic | All Rights Reserved | https://igorlekic.com/
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DzX_RadialMenu.generated.h"

UENUM(BlueprintType)
enum class EWidgetCenter : uint8
{
	ScreenCenter 		UMETA(DisplayName = "Center of the screen"),
	MousePosition		UMETA(DisplayName = "Mouse position")

};

USTRUCT(BlueprintType)
struct FRadialDivisions
{
	GENERATED_BODY()

		//How many buttons menu needs to have.
		UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Parameters")
		int32 NoDivisions = 1;
	//Minimal angle of one button
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Parameters")
		float MinAngleOfSegment;
	//Radius of a dead zone from the center
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Parameters")
		float DeadZoneRadius;
	/*Maximal angle of one button.
	Leave 360 to split full circle into segments.*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Parameters")
		float MaxAngleOfSegment;
	//Container for lower boarders of buttons. int32-button position, float - min angle.
	UPROPERTY(BlueprintReadOnly, Category = "Parameters")
		TMap<int32, float>MinMap;
	//Container for upper boarders of buttons. int32-button position, float - max angle.
	UPROPERTY(BlueprintReadOnly, Category = "Parameters")
		TMap<int32, float>MaxMap;
	//Check if menu is divided
	UPROPERTY(BlueprintReadOnly, Category = "Parameters")
		bool bIsMenuDivided = false;
	//Getting min and max values. Spliting 360deg into segments.
	void GetMinAndMax()
	{
		//Size of one button in deg
		float AngleRangePerSegment;
		//Split only if there is 1 button or more.
		if (NoDivisions > 0)
		{
			//Calculate size of button in regard to the full circle
			AngleRangePerSegment = 360 / NoDivisions;
			//If one button angle size is larger then minimal angle, buttons can be made.
			if (AngleRangePerSegment >= MinAngleOfSegment)
			{
				//If angle of one button is larger then maximal angle, cap to max angle.
				if (AngleRangePerSegment > MaxAngleOfSegment)
				{
					AngleRangePerSegment = MaxAngleOfSegment;
				}
				//Make menu button 0 do be in the middle of circle beginning/end.
				//Minimal angle is 360deg-half button size
				MinMap.Add(0, 360 - AngleRangePerSegment / 2);
				//Maximal angle is 360deg + half button size
				MaxMap.Add(0, AngleRangePerSegment / 2);
				//if there is more then 1 division ( 1st division is button 0) loop rest.
				for (int32 i = NoDivisions - 1; i > 0; i--)
				{
					//Adding min value for button i in MinMap
					MinMap.Add(i, i * AngleRangePerSegment - AngleRangePerSegment / 2);
					//Adding max value for button i in MaxMap
					MaxMap.Add(i, (i * AngleRangePerSegment - AngleRangePerSegment / 2) + AngleRangePerSegment);
				}
			}
		}
		//If divisions are made change bool check
		bIsMenuDivided = true;
	};
	//Getting current segment in regard to current angle of the mouse.
	int32 CurrentSegment(float Angle)
	{
		//Loop only if menu is divided.
		if (bIsMenuDivided)
		{
			/*Getting menu button on position 0
			Because menu button 0 is split on two halves on circle beginning/end we need two condition checks.
			*/
			if (Angle > MinMap[0] || Angle < MaxMap[0])
			{
				return 0;
			}
			//Loop buttons on positions greater then 0. If angle is in min and max boarders return current button position.
			for (int32 i = MaxMap.Num() - 1; i > 0; i--)
			{
				if (MinMap[i] <= Angle && Angle <= MaxMap[i])
				{
					return i;
				}
			}
		}
		//Return -1 if condition is not met
		return -1;
	};
	//Constructor
	FRadialDivisions()
	{
		NoDivisions = 1;
		MinAngleOfSegment = 10.f;
		MaxAngleOfSegment = 360.f;
		DeadZoneRadius = 0.f;
	}
};
/**
 * 
 */
UCLASS()
class RADIALMENU_API UDzX_RadialMenu : public UUserWidget
{
	GENERATED_BODY()

public:
	// Called every frame
	virtual void NativeTick(const FGeometry& MyGeometry, float DeltaTime) override;
#pragma region Variables
private:
	UPROPERTY()
		class APlayerController* PlayerController;

	UPROPERTY()
		int32 CurrentSegmentID;

public:
	/*Is this menu active.
	Useful when there are several instances of this class in one widget.*/
	UPROPERTY(BlueprintReadOnly, Category = "Parameters")
		bool bIsMenuActive = false;
	//Struct with parameters and functions
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameters")
		FRadialDivisions DivideMenu;
	//Angle to rotate slot 0 in menu. By default its on 12h position.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameters")
		float RotateMenuStart;
	//Clockwise or count.clockwise rotation of the menu.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameters")
		bool bIsClockwise = true;

	//From which point plugin calculate center of the widget
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameters")
	EWidgetCenter CalculateCenterFrom = EWidgetCenter::ScreenCenter;

	/*Center of the widget, get calculated each time widget is constructed.
	Top left corner is 0,0. X+ is to the right, Y+ is to downward.
	*/
	UPROPERTY(BlueprintReadOnly, Category = "Parameters")
	FVector2D RadialMenuCenter;
	/* Offset from actual center. X+ to the right. Y+ upward.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameters")
		FVector2D CenterOffset = FVector2D(0,0);

public:
	//Collection of all widget radial buttons.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Buttons")
		TMap<int32, class UDzX_RadialMenu_Button*>Buttons;

#pragma endregion Variables


#pragma region Functions
protected:
	virtual bool Initialize() override;
public:
	//Getting center of the widget
	UFUNCTION(Category = "Functions")
		void SetCenter();
	//Getting angle from the center of the screen
	UFUNCTION(BlueprintCallable, Category = "Functions")
		float GetAngleFromCenter();
	//Divide 360deg into segments - need to call only once.
	UFUNCTION(BlueprintCallable,Category = "Functions")
		void DivideIntoSegments();
	//Getting current segment - DivideIntoSegments needs to be called 1st
	UFUNCTION(BlueprintCallable, Category = "Functions")
		int32 GetCurrentSegment(float Angle);

	//Setup menu.
	UFUNCTION(BlueprintCallable, Category = "Functions")
		void Setup();

	//Calls blueprint native event inside this widget
	UFUNCTION(BlueprintNativeEvent, Category = "Functions")
		void OnSetup();
	//Setup menu.
	UFUNCTION(BlueprintCallable, Category = "Functions")
		void SetMenuActive(bool bIsActive);
	//Destroy menu.
	UFUNCTION(BlueprintCallable, Category = "Functions")
		void Destructor();


	//Gets all radial buttons in this widget
	UFUNCTION(Category = "Functions")
		void GetButtons();
	//Make hover effect over the button
	UFUNCTION(Category = "Functions")
		void PreselectButton();

	/*Make hover effect over the button - Gamepad
	*@ThumbValue_X - Value between -1 and 1 depending on thumbstick leaning left or right 
	*@ThumbValue_Y - Value between -1 and 1 depending on thumbstick leaning up or down 
	*/
	UFUNCTION(BlueprintCallable,Category = "Functions")
		void PreselectButtonGamepad(float ThumbValue_X, float ThumbValue_Y);
	//Check if mouse is in dead zone
	UFUNCTION(Category = "Functions")
		bool CheckForDeadZone();
	//Makes button active
	UFUNCTION(BlueprintCallable, Category = "Functions")
		void ActivateButton(int32 ActivateIndex);
	//Makes button disabled
	UFUNCTION(BlueprintCallable, Category = "Functions")
		void DisableButton(int32 ActivateIndex);
	//Deactivate all buttons
	UFUNCTION(BlueprintCallable, Category = "Functions")
		void DeactivateButtons();

	//Calls click event on interface
	UFUNCTION(BlueprintCallable, Category = "Functions")
		virtual void ClickEvent();
	//Calls hover event on interface
	UFUNCTION(Category = "Functions")
		virtual void HoverOverEvent();
	//Calls blueprint native event inside this widget
	UFUNCTION(BlueprintNativeEvent, Category = "Functions")
		void OnClicked();
	//Calls blueprint native event inside this widget
	UFUNCTION(BlueprintNativeEvent, Category = "Functions")
		void OnHoverOver();
	//Set default bindings for this widget (left click is select)
	void DefaultBinding();

	//Manually enter button angles after setup.
	UFUNCTION(BlueprintCallable, Category = "Functions")
		void ManualButtonOverride(int32 ButtonID,float MinAngle, float MaxAngle);

#pragma endregion Functions
	/************************************************************************/
	/* Interface                                                            */
	/************************************************************************/
#pragma region Interface
public:
	//Set interface for an object of this class
	UFUNCTION(BlueprintCallable, Category = "Functions")
		void SetRadMenuInterface(TScriptInterface<IDzX_RadialMenu_Interface> _MenuInterface);


	//Interface reference
	IDzX_RadialMenu_Interface* RadialInterface;
	//Object that contain interface reference
	UObject* RadialInterfaceObj;

#pragma endregion Interface

};
