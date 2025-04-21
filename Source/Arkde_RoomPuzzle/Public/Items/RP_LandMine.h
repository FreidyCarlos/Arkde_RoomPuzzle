// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RP_LandMine.generated.h"

class UStaticMeshComponent;
class USphereComponent;
class URP_HealthComponent;
class UParticleSystem;
class USoundBase;

UCLASS()
class ARKDE_ROOMPUZZLE_API ARP_LandMine : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARP_LandMine();

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* MineMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USphereComponent* AlertTrigger;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USphereComponent* ExplosionTrigger;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	URP_HealthComponent* HealthComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UAudioComponent* AlertAudioComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "MineConfig", meta = (ClampMin = 0.0))
	float ExplosionDamage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "MineConfig")
	TSubclassOf<UDamageType> DamageTypeClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effects")
	UParticleSystem* ExplosionEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effects")
	USoundBase* ExplosionSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effects")
	UParticleSystem* AlertEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effects")
	USoundBase* AlertLoopSound;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	bool bHasExploded;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnAlertTriggerOverlap(UPrimitiveComponent* OverlappedComponent,AActor* OtherActor,UPrimitiveComponent* OtherComp,int32 OtherBodyIndex,bool bFromSweep,const FHitResult& SweepResult);

	UFUNCTION()
	void OnAlertTriggerEndOverlap(UPrimitiveComponent* OverlappedComponent,AActor* OtherActor,UPrimitiveComponent* OtherComp,int32 OtherBodyIndex);

	UFUNCTION()
	void OnExplosionTriggerOverlap(UPrimitiveComponent* OverlappedComponent,AActor* OtherActor,UPrimitiveComponent* OtherComp,int32 OtherBodyIndex,bool bFromSweep,const FHitResult& SweepResult);

	UFUNCTION()
	void OnMineHealthChanged(URP_HealthComponent* OwningHealthComp,AActor* DamagedActor,float Damage,const class UDamageType* DamageType,class AController* InstigatedBy,AActor* DamageCauser);

	void Explode();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
