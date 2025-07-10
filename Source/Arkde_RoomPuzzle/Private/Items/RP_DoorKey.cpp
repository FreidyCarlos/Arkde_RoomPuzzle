// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/RP_DoorKey.h"
#include "Components/StaticMeshComponent.h"
#include "RP_Character.h"
#include "Core/RP_GameMode.h"

ARP_DoorKey::ARP_DoorKey()
{
	KeyMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("KeysMeshComponent"));
	KeyMeshComponent->SetupAttachment(RootComponent);
	KeyMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	KeyTag = "KeyA";
}

void ARP_DoorKey::PickUp(ARP_Character* PickupCharacter)
{
	Super::PickUp(PickupCharacter);

	if (IsValid(PickupCharacter) && PickupCharacter->GetCharacterType() == ERP_CharacterType::CharacterType_Player)
	{
		if (IsValid(GameModeReference))
		{
			GameModeReference->AddKeyToCharacter(PickupCharacter, KeyTag);
		}
		Destroy();
	}
}
