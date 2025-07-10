// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUD/RP_HUDUltimate.h"
#include "Kismet/GameplayStatics.h"
#include "RP_Character.h"

void URP_HUDUltimate::InitializeWidget()
{
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (IsValid(PlayerPawn))
	{
		ARP_Character* PlayerCharacter = Cast<ARP_Character>(PlayerPawn);
		if (IsValid(PlayerCharacter))
		{
			PlayerCharacter->OnUltimateUpdateDelegate.AddDynamic(this, &URP_HUDUltimate::UpdateUltimateValue);
			PlayerCharacter->OnUltimateStatusDelegate.AddDynamic(this, &URP_HUDUltimate::UpdateUltimateStatus);

			UpdateUltimateValue(PlayerCharacter->GetCurrentUltimateXP(), PlayerCharacter->GetMaxUltimateXP());
			UpdateUltimateStatus(PlayerCharacter->CanUseUltimate());
		}
	}
}

void URP_HUDUltimate::UpdateUltimateValue(float WCurrentUltimateXP, float WMaxUltimateXP)
{
	UltimatePercent = WCurrentUltimateXP / WMaxUltimateXP;
}

void URP_HUDUltimate::UpdateUltimateStatus(bool bIsAvalaible)
{
	UltimateColor = bIsAvalaible ? UltimateEnabledColor : UltimateDisabledColor;
}
