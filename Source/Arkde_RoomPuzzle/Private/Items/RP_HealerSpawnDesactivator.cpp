// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/RP_HealerSpawnDesactivator.h"
#include "Components/StaticMeshComponent.h"
#include "RP_Character.h"
#include "Enemy/RP_HealerSpawner.h"

ARP_HealerSpawnDesactivator::ARP_HealerSpawnDesactivator()
{
	DesactivatorMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DesactivatorMeshComponent"));
	DesactivatorMeshComponent->SetupAttachment(RootComponent);
	DesactivatorMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ARP_HealerSpawnDesactivator::SetIsPick(bool bPick)
{
	bIsPick = bPick;
}

void ARP_HealerSpawnDesactivator::PickUp(ARP_Character* PickupCharacter)
{
	Super::PickUp(PickupCharacter);

	bool bSuccesfullPickupDesactivator = PickupCharacter->TryDesactivateBotSpawn(bIsPick);

	if (IsValid(OwningSpawner))
	{
		OwningSpawner->SetSpawnActive(false);
	}
	Destroy();
}
