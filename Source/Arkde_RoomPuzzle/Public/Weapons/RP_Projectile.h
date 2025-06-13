// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RP_Projectile.generated.h"

class UStaticMeshComponent;
class UProjectileMovementComponent;
class USphereComponent;
class UParticleSystem;


UCLASS()
class ARKDE_ROOMPUZZLE_API ARP_Projectile : public AActor
{
	GENERATED_BODY()

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USphereComponent* ProjectileCollision;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* ProjectileMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UProjectileMovementComponent* ProjectileMovementComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	float ExplosionDelay; 

	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	float ExplosionRadius; 

	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	float ExplosionDamage;

	FTimerHandle ExplosionTimerHandle;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage")
	TSubclassOf<UDamageType> DamageType;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	UParticleSystem* ImpactEffect;

public:	
	// Sets default values for this actor's properties
	ARP_Projectile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	void Explode(); 

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
