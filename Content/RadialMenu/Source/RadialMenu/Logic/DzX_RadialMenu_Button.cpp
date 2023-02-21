// Copyright 2020 Igor Lekic | All Rights Reserved | https://igorlekic.com/

#include "DzX_RadialMenu_Button.h"
#include "DzX_RadialMenu_Interface.h"

bool UDzX_RadialMenu_Button::Initialize()
{
	if (!Super::Initialize())
	{
		return false;
	}
	ChangeCurrentState(EButtonState::Normal);
	return true;
}

//Change look of this button to new state
void UDzX_RadialMenu_Button::ChangeCurrentState(EButtonState NewState)
{
	//If image of the button exist
	if (ButtonLook)
	{
		//Change current state variable to new state.
		CurrentButtonState = NewState;
		//Switch look in regard to the new state.
		switch (NewState)
		{
		case EButtonState::Active:
			ButtonLook->SetBrush(Active);
			break;
		case EButtonState::Normal:
			ButtonLook->SetBrush(Normal);
			break;
		case EButtonState::Disabled:
			ButtonLook->SetBrush(Disabled);
			break;
		case EButtonState::HoveredNormal:
			ButtonLook->SetBrush(HoveredNormal);
			break;
		case EButtonState::HoveredDisabled:
			ButtonLook->SetBrush(HoveredDisabled);
			break;
		default:
			CurrentButtonState = EButtonState::Disabled;
			ButtonLook->SetBrush(Disabled);
			break;
		}
	}

}
//Refresh button look
void UDzX_RadialMenu_Button::Refresh()
{
	//If image of the button exist
	if (ButtonLook)
	{
		//Switch look in regard to the current button state.
		switch (CurrentButtonState)
		{
		case EButtonState::Active:
			ButtonLook->SetBrush(Active);
			break;
		case EButtonState::Normal:
			ButtonLook->SetBrush(Normal);
			break;
		case EButtonState::Disabled:
			ButtonLook->SetBrush(Disabled);
			break;
		case EButtonState::HoveredNormal:
			ButtonLook->SetBrush(HoveredNormal);
			break;
		case EButtonState::HoveredDisabled:
			ButtonLook->SetBrush(HoveredDisabled);
			break;
		default:
			CurrentButtonState = EButtonState::Disabled;
			ButtonLook->SetBrush(Disabled);
			break;
		}
	}
}
//Button clicked event
void UDzX_RadialMenu_Button::ClickEvent()
{
	//Call for blueprint event
	OnClicked();
}
void UDzX_RadialMenu_Button::OnClicked_Implementation()
{

}
