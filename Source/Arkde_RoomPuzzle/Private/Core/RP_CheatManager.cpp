// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/RP_CheatManager.h"
#include "Kismet/GameplayStatics.h"
#include "RP_Character.h"

void URP_CheatManager::RP_UltimateReady()
{
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);

	if (IsValid(PlayerPawn))
	{
		ARP_Character* PlayerCharacter = Cast<ARP_Character>(PlayerPawn);
		if (IsValid(PlayerCharacter) && PlayerCharacter->GetCharacterType() == ERP_CharacterType::CharacterType_Player)
		{
			PlayerCharacter->GainUltimateXP(10000);
		}
	}
}
