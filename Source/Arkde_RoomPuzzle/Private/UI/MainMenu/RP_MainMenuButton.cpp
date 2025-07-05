// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MainMenu/RP_MainMenuButton.h"

URP_MainMenuButton::URP_MainMenuButton()
{
	CheckFocusRate = 0.1f;
}

void URP_MainMenuButton::Start()
{
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_CheckFocus, this, &URP_MainMenuButton::CheckFocus, CheckFocusRate, true);
}

void URP_MainMenuButton::CheckFocus()
{
	if (IsHovered())
	{
		//Boton afectado por el mouse
		SetKeyboardFocus();
		SetButtonStyle(ERP_ButtonStyleType::ButtonStyleType_Mouse);
	}
	else
	{
		//Validar si el teclado o el control esta disparando la funcion 
		ERP_ButtonStyleType StyleTypeSelected = HasKeyboardFocus() ? ERP_ButtonStyleType::ButtonStyleType_KeySelected : ERP_ButtonStyleType::ButtonStyleType_KeyNotSelected;
		SetButtonStyle(StyleTypeSelected);
	}

}

void URP_MainMenuButton::SetButtonStyle(ERP_ButtonStyleType NewStyleType)
{
	BP_SetButtonStyle(NewStyleType);
}
