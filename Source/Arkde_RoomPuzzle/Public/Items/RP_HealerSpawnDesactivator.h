// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/RP_Item.h"
#include "RP_HealerSpawnDesactivator.generated.h"

class UStaticMeshComponent;
class ARP_HealerSpawner;

/**
 * 
 */
UCLASS()
class ARKDE_ROOMPUZZLE_API ARP_HealerSpawnDesactivator : public ARP_Item
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* DesactivatorMeshComponent;

protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Desactivator")
	bool bIsPick;

	UPROPERTY()
    ARP_HealerSpawner* OwningSpawner;

public:

	// Sets default values for this actor's properties
	ARP_HealerSpawnDesactivator();

public:

	void SetOwningSpawner(ARP_HealerSpawner* Spawner) { OwningSpawner = Spawner; }

	UFUNCTION(BlueprintCallable, Category="Desactivator")
    void SetIsPick(bool bPick);

protected:

	virtual void PickUp(ARP_Character* PickupCharacter) override;
	
};
