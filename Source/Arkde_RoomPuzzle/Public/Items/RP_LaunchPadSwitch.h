// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/RP_Item.h"
#include "RP_LaunchPadSwitch.generated.h"

class ARP_LaunchPad;
class UStaticMeshComponent;

/**
 * 
 */
UCLASS()
class ARKDE_ROOMPUZZLE_API ARP_LaunchPadSwitch : public ARP_Item
{
	GENERATED_BODY()

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* ButtonMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LaunchPad Switch")
	ARP_LaunchPad* LinkedLaunchPad;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LaunchPad Switch")
	bool bIsLaunchPadActive = false;


public:
	// Sets default values for this actor's properties
	ARP_LaunchPadSwitch();


protected:

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "LaunchPad Switch")
	void LaunchPadButton(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
};
