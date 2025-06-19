// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/RP_HealthBox.h"
#include "Components/StaticMeshComponent.h"
#include "RP_Character.h"

ARP_HealthBox::ARP_HealthBox()
{
	HealthBoxMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HealthBoxMeshComponent"));
	HealthBoxMeshComponent->SetupAttachment(RootComponent);
	HealthBoxMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	HealthValue = 15.0f;
}

void ARP_HealthBox::PickUp(ARP_Character* PickupCharacter)
{
	Super::PickUp(PickupCharacter);

	bool bSuccesfullHeal = PickupCharacter->TryAddHealth(HealthValue);

	if (bSuccesfullHeal)
	{
		Destroy();
	}
}
