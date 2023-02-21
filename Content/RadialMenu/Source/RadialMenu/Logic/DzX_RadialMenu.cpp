// Author: Igor Lekic - https://igorlekic.com/

#include "DzX_RadialMenu.h"
#include "Engine.h"
#include "GameFramework/PlayerController.h"
#include "DzX_RadialMenu_Button.h"
#include "DzX_RadialMenu_Interface.h"
#include "Blueprint/WidgetTree.h"


//Tick
void UDzX_RadialMenu::NativeTick(const FGeometry& MyGeometry, float DeltaTime)
{
	Super::NativeTick(MyGeometry, DeltaTime);
	//Check if menu is active
	if (bIsMenuActive)
	{
		//Update hover effect each frame
		PreselectButton();
	}

}
//Init
bool UDzX_RadialMenu::Initialize()
{
	if (!Super::Initialize())
	{
		return false;
	}
	//Get player controller
	PlayerController = Cast<APlayerController>(GetOwningPlayer());

	return true;
}
/*Setup this widget, have to be called (trough blueprint or parent class) to activate this widget.
BlueprintCallable
*/
void UDzX_RadialMenu::Setup()
{
	//Initialize input component for key bindings.
	InitializeInputComponent();
	//Divide widget into radial segments
	DivideIntoSegments();
	//Get all radial buttons into a TMap
	GetButtons();
	//Set default bindings
	DefaultBinding();
	//Activate this menu.
	SetMenuActive(true);
	//Get cente for radial calculation
	SetCenter();
	//CallsBlueprintEvent
	OnSetup();

}

void UDzX_RadialMenu::OnSetup_Implementation()
{

}

/*Set active state for this menu.
BlueprintCallable
*/
void UDzX_RadialMenu::SetMenuActive(bool bIsActive)
{
	//Set local variable.
	bIsMenuActive = bIsActive;
}
//Destructor for this widget
void UDzX_RadialMenu::Destructor()
{
	//Check if there is input component for key bidning
	if (InputComponent)
	{
		//Clear any binding from this object
		InputComponent->KeyBindings.Empty();
	}
	//Deactivate menu
	SetMenuActive(false);
	//Call parent destruct
	Destruct();
}
//Gets all the buttons from this widget
void UDzX_RadialMenu::GetButtons()
{
	//Clear buttons container
	Buttons.Empty();
	//Gets widget tree for all objects(Widgets) in blueprint
	WidgetTree->ForEachWidget([&](UWidget* Widget)
	{
		//Check if widget exist
		check(Widget);
		//Check if widget is a radial button
		if (Widget->IsA(UDzX_RadialMenu_Button::StaticClass()))
		{
			//Cast widget to radial button
			UDzX_RadialMenu_Button* Button = Cast<UDzX_RadialMenu_Button>(Widget);
			if (Button && Button->ButtonPosition >= 0)
			{
				if (Buttons.Contains(Button->ButtonPosition))
				{
					GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Buttons in this menu have same ID. ButtonPosition should be unique number for each button.")));

				}
				//Add button to TMap where Index is button position(manually setup for each button)
				Buttons.Add(Button->ButtonPosition, Button);

			}
		}
	});
}
/*Hover effect
BlueprintCallable
*/
void UDzX_RadialMenu::PreselectButton()
{
	//Check if menu is active
	if (bIsMenuActive && CheckForDeadZone())
	{		
		//Locally get current segment ID from given angle. (mouse position in regard to the center of the screen)
		int32 _tempSegment = GetCurrentSegment(GetAngleFromCenter());
		/*Compare if current segment is not equal to lastly updated segment(CurrentSegmentID) to avoid looping each frame if there is no change.
		Also check if button exist in Buttons container to avoid errors.
		*/
		if (_tempSegment != CurrentSegmentID && Buttons.Contains(_tempSegment))
		{
			//If there is change, update CurrentSegmentID to new segment ID
			CurrentSegmentID = _tempSegment;
			HoverOverEvent();
			//Loop all buttons
			for (auto& _Button : Buttons)
			{
				//Switch all from hovered to NOT hovered state
				switch (_Button.Value->CurrentButtonState)
				{
				case EButtonState::HoveredNormal:
					_Button.Value->ChangeCurrentState(EButtonState::Normal);
					break;
				case EButtonState::HoveredDisabled:
					_Button.Value->ChangeCurrentState(EButtonState::Disabled);
					break;
				}
			}
			//Switch current button in segment to hovered
			switch (Buttons[_tempSegment]->CurrentButtonState)
			{
			case EButtonState::Disabled:
				Buttons[_tempSegment]->ChangeCurrentState(EButtonState::HoveredDisabled);
				break;
			case EButtonState::Normal:
				Buttons[_tempSegment]->ChangeCurrentState(EButtonState::HoveredNormal);
				break;
			}
		}
	}
}


void UDzX_RadialMenu::PreselectButtonGamepad(float ThumbValue_X, float ThumbValue_Y)
{
	//Create Vector from X and Y
	FVector GamePadPosition = FVector(ThumbValue_X, ThumbValue_Y,0);
	//Check if thumbstick is moved
	if (GamePadPosition.Size()>0.00f)
	{
		//Get rotation from thumbstick vector
		FRotator Angle = GamePadPosition.Rotation();
		//Check if menu is active
		if (bIsMenuActive)
		{
			//Get angle from thumbstick and adjust it to parameters of the menu
			float AimAtAngle = ((Angle.Yaw-90) + (bIsClockwise ? -180 : 180)) * (bIsClockwise ? -1 : 1);

			//Locally get current segment ID from given angle.
			int32 _tempSegment = GetCurrentSegment(AimAtAngle);
			/*Compare if current segment is not equal to lastly updated segment(CurrentSegmentID) to avoid looping each frame if there is no change.
			Also check if button exist in Buttons container to avoid errors.
			*/
			if (_tempSegment != CurrentSegmentID && Buttons.Contains(_tempSegment))
			{
				//If there is change, update CurrentSegmentID to new segment ID
				CurrentSegmentID = _tempSegment;
				HoverOverEvent();
				//Loop all buttons
				for (auto& _Button : Buttons)
				{
					//Switch all from hovered to NOT hovered state
					switch (_Button.Value->CurrentButtonState)
					{
					case EButtonState::HoveredNormal:
						_Button.Value->ChangeCurrentState(EButtonState::Normal);
						break;
					case EButtonState::HoveredDisabled:
						_Button.Value->ChangeCurrentState(EButtonState::Disabled);
						break;
					}
				}
				//Switch current button in segment to hovered
				switch (Buttons[_tempSegment]->CurrentButtonState)
				{
				case EButtonState::Disabled:
					Buttons[_tempSegment]->ChangeCurrentState(EButtonState::HoveredDisabled);
					break;
				case EButtonState::Normal:
					Buttons[_tempSegment]->ChangeCurrentState(EButtonState::HoveredNormal);
					break;
				}
			}
		}
	}
}

bool UDzX_RadialMenu::CheckForDeadZone()
{
	if (PlayerController)
	{
		//X and Y coordinates for mouse
		float MouseX;
		float MouseY;

		PlayerController->GetMousePosition(MouseX, MouseY);
		
		//Make 2D vector of mouse position
		const FVector2D MousePos = FVector2D(MouseX, MouseY);

		float MouseDistanceFromCenter = FVector2D(MousePos - RadialMenuCenter).Size();

		if (MouseDistanceFromCenter > DivideMenu.DeadZoneRadius)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false; 
	}
}

/*
Activate button.
*/
void UDzX_RadialMenu::ActivateButton(int32 ActivateIndex)
{
	//If menu is active and button exist
	if (bIsMenuActive && Buttons.Contains(ActivateIndex))
	{
		Buttons[ActivateIndex]->ChangeCurrentState(EButtonState::Active);

	}

}

void UDzX_RadialMenu::DisableButton(int32 ActivateIndex)
{
	//If menu is active and button exist
	if (bIsMenuActive && Buttons.Contains(ActivateIndex))
	{
		//Disable that button
		Buttons[ActivateIndex]->ChangeCurrentState(EButtonState::Disabled);
	}
}

void UDzX_RadialMenu::DeactivateButtons()
{
	//Loop all buttons
	for (auto& _Button : Buttons)
	{
		//If button was previously active and object is not this button, then deactivate it
		if (_Button.Value->CurrentButtonState == EButtonState::Active)
		{
			//Change state to normal
			_Button.Value->ChangeCurrentState(EButtonState::Normal);
		}
	}
}

//Called from LMB click bind
void UDzX_RadialMenu::ClickEvent()
{
	//If menu is active
	if (bIsMenuActive)
	{
		// Try to Execute on C++ layer:
		if (RadialInterface)
		{
			RadialInterface->Execute_RadialButtonClick(RadialInterfaceObj, this);
		}
		else
		{
			// Else, Execute Interface on Blueprint layer instead:
			if (RadialInterfaceObj && RadialInterfaceObj->GetClass()->ImplementsInterface(UDzX_RadialMenu_Interface::StaticClass()))
			{
				IDzX_RadialMenu_Interface::Execute_RadialButtonClick(RadialInterfaceObj, this);
			}
		}
	}
	//Check if button on current ID exist
	if (Buttons.Contains(CurrentSegmentID))
	{
		//Call click event for that button.
		Buttons[CurrentSegmentID]->ClickEvent();
	}
	//Calls blueprint native event inside this widget
	OnClicked();
}

void UDzX_RadialMenu::HoverOverEvent()
{
	//If menu is active
	if (bIsMenuActive)
	{
		// Try to Execute on C++ layer:
		if (RadialInterface)
		{
			RadialInterface->Execute_RadialMenuHoverOver(RadialInterfaceObj, this);
		}
		else
		{
			// Else, Execute Interface on Blueprint layer instead:
			if (RadialInterfaceObj && RadialInterfaceObj->GetClass()->ImplementsInterface(UDzX_RadialMenu_Interface::StaticClass()))
			{
				IDzX_RadialMenu_Interface::Execute_RadialMenuHoverOver(RadialInterfaceObj, this);
			}
		}
	}
	OnHoverOver();
}

//Calls blueprint native event inside this widget
void UDzX_RadialMenu::OnClicked_Implementation()
{

}

void UDzX_RadialMenu::OnHoverOver_Implementation()
{

}

//Default bindings for this widget
void UDzX_RadialMenu::DefaultBinding()
{
	if (InputComponent)
	{
		InputComponent->BindKey(EKeys::LeftMouseButton, IE_Pressed, this, &UDzX_RadialMenu::ClickEvent);
	}
}

void UDzX_RadialMenu::ManualButtonOverride(int32 ButtonID, float MinAngle, float MaxAngle)
{
	DivideMenu.MaxMap.Add(ButtonID, MaxAngle);
	DivideMenu.MinMap.Add(ButtonID, MinAngle);
}


//Setup interface
void UDzX_RadialMenu::SetRadMenuInterface(TScriptInterface<IDzX_RadialMenu_Interface> _MenuInterface)
{
	//Get reference to interface from calling object
	RadialInterface = Cast<IDzX_RadialMenu_Interface>(_MenuInterface.GetObject());
	//Get reference to calling object
	RadialInterfaceObj = _MenuInterface.GetObjectRef();
}
//Getting center of the widget
void UDzX_RadialMenu::SetCenter()
{
	//Check for controller
	if (PlayerController)
	{

		switch (CalculateCenterFrom)
		{
		case EWidgetCenter::ScreenCenter:
		{
			//X and Y coordinates for viewport
			int32 ViewportSizeX;
			int32 ViewportSizeY;

			//Get Viewport coordinates
			PlayerController->GetViewportSize(ViewportSizeX, ViewportSizeY);

			//Make 2D vector of viewport
			const FVector2D ViewportSize = FVector2D(ViewportSizeX, ViewportSizeY);
			//Viewport Center!            
			const FVector2D  ViewportCenter = FVector2D(ViewportSize.X / 2, ViewportSize.Y / 2);
			const FVector2D OffsetedCenter = FVector2D(ViewportCenter.X + CenterOffset.X, ViewportCenter.Y - CenterOffset.Y);
			//Set the local variable
			RadialMenuCenter = OffsetedCenter;
		}
		break;

		case EWidgetCenter::MousePosition:
		{
			//X and Y coordinates for mouse
			float MouseX;
			float MouseY;
			//Get mouse coordinates
			PlayerController->GetMousePosition(MouseX, MouseY);

			//Make 2D vector of mouse position
			const FVector2D MousePos = FVector2D(MouseX + CenterOffset.X, MouseY - CenterOffset.Y);

			//Set the local variable
			RadialMenuCenter = MousePos;

		}
		break;
		}
	}

}

//Get angle from center of the screen
float  UDzX_RadialMenu::GetAngleFromCenter()
{
	//Check for controller
	if (!PlayerController)
	{
		PlayerController = Cast<APlayerController>(GetOwningPlayer());
	}
	else
	{
		//X and Y coordinates for mouse
		float MouseX;
		float MouseY;
		
		PlayerController->GetMousePosition(MouseX, MouseY);

		//Make 2D vector of mouse position
		const FVector2D MousePos = FVector2D(MouseX, MouseY);

		/*Mouse position reduced by screen center to translate vector in regard to coordinate center
		Then rotate it if center is not at 12PM position.
		*/
		const FVector2D ReducedVector = FVector2D(MousePos - RadialMenuCenter).GetRotated(RotateMenuStart);
		//Getting angle of vector
		float Alpha = FMath::Atan2(ReducedVector.X, ReducedVector.Y);
		/*Conversion to Degrees and add/sub 180 to round up for 360 degrees,
		then multiply by 1 or -1 to get positive numbers in regard to clockwise or counter clockwise rotation of the menu.*/
		float AimAtAngle = ((FMath::RadiansToDegrees(Alpha)) + (bIsClockwise ? -180 : 180)) * (bIsClockwise ? -1 : 1);
		//Return value
		return AimAtAngle;
	}
	return 0.f;
}

void UDzX_RadialMenu::DivideIntoSegments()
{
	//Calls struct function to divide circle into segments.
	DivideMenu.GetMinAndMax();

}

int32 UDzX_RadialMenu::GetCurrentSegment(float Angle)
{
	//Get current segment from an angle
	float Segment = DivideMenu.CurrentSegment(Angle);
	return Segment;
}
