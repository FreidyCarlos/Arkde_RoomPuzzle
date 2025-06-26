// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/RP_SpawnDesactivator.h"
#include "Components/StaticMeshComponent.h"
#include "RP_Character.h"
#include "Enemy/RP_BotSpawner.h"

ARP_SpawnDesactivator::ARP_SpawnDesactivator()
{
	DesactivatorMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DesactivatorMeshComponent"));
	DesactivatorMeshComponent->SetupAttachment(RootComponent);
	DesactivatorMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ARP_SpawnDesactivator::SetIsPick(bool bPick)
{
	bIsPick = bPick;
}

void ARP_SpawnDesactivator::PickUp(ARP_Character* PickupCharacter)
{
	Super::PickUp(PickupCharacter);

	bool bSuccesfullPickupDesactivator = PickupCharacter->TryDesactivateBotSpawn(bIsPick);

	if (IsValid(OwningSpawner))
	{
		OwningSpawner->SetSpawnActive(false);
	}
	Destroy();
}
