// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RP_HealthComponent.generated.h"


UCLASS( ClassGroup=(ROOM), meta=(BlueprintSpawnableComponent) )
class ARKDE_ROOMPUZZLE_API URP_HealthComponent : public UActorComponent
{
	GENERATED_BODY()

protected:

	UPROPERTY(BlueprintReadWrite, Category = "HealthComponent")
	float Health;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HealthComponent", meta = (ClampMin = 0.0, UIMin = 0.0))
	float MaxHealth;

	UPROPERTY(BlueprintReadOnly, Category = "HealthComponent")
	AActor* MyOwner;

public:	
	// Sets default values for this component's properties
	URP_HealthComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;		

	UFUNCTION()
	void TakingDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);
};
