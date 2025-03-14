// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RP_LaunchPad.generated.h"

class UBoxComponent;
class UStaticMeshComponent;

UCLASS()
class ARKDE_ROOMPUZZLE_API ARP_LaunchPad : public AActor
{
	GENERATED_BODY()

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UBoxComponent* CollisionBox;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* PlatformMesh;

protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LaunchPad Settings")//E'ta vaina es la velocidad de catapulta (de momento hacia arriba, pero se puede cambiar en Unreal)
	FVector LaunchVelocity = FVector(0.0f, 0.0f, 1000.0f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LaunchPad Settings")
	bool bIsEnabled = false;
	
public:	
	// Sets default values for this actor's properties
	ARP_LaunchPad();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnLaunchPadActivated(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SetLaunchPadEnabled(bool bNewState);

};
