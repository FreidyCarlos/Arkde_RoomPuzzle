// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BP_ExplosiveBarrel.generated.h"

class UStaticMeshComponent;
class USphereComponent;
class URP_HealthComponent;

UCLASS()
class ARKDE_ROOMPUZZLE_API ABP_ExplosiveBarrel : public AActor
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* BarrelMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USphereComponent* OverlapSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	URP_HealthComponent* HealthComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effects")
	UParticleSystem* ExplosionEffect;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Burn")
	float BurnDamagePerSecond;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Burn")
	float BurnDurationSeconds;

public:	
	// Sets default values for this actor's properties
	ABP_ExplosiveBarrel();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
    void HandleHealthChanged(URP_HealthComponent* OwningHealthComp,AActor* DamagedActor,float Damage, const UDamageType* DamageType,AController* InstigatedBy,AActor* DamageCauser);

	void Explode();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
