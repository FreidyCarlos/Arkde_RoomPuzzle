// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RP_HealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_SixParams(FOnHealthChangeSignature, URP_HealthComponent*, HealthComponent, AActor*, DamagedActor, float, Damage, const class UDamageType*, DamageType, class AController*, InstigatedBy, AActor*, DamageCauser);

UCLASS( ClassGroup=(ROOM), meta=(BlueprintSpawnableComponent) )
class ARKDE_ROOMPUZZLE_API URP_HealthComponent : public UActorComponent
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Debug")
	bool bDebug;

	UPROPERTY(BlueprintReadOnly, Category = "HealthComponent")
	bool bIsDead;

	UPROPERTY(BlueprintReadOnly, Category = "HealthComponent")
	bool bInvulnerableState;

	UPROPERTY(BlueprintReadWrite, Category = "HealthComponent")
	float Health;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HealthComponent", meta = (ClampMin = 0.0, UIMin = 0.0))
	float MaxHealth;

	UPROPERTY(BlueprintReadOnly, Category = "HealthComponent")
	AActor* MyOwner;

public:

	UPROPERTY(BlueprintAssignable)
	FOnHealthChangeSignature OnHealthChangeDelegate;

	UFUNCTION(BlueprintCallable, Category="Health")
    float GetHealth() const;

    UFUNCTION(BlueprintCallable, Category="Health")
    float GetMaxHealth() const;

	UFUNCTION(BlueprintCallable, Category="Health")
    void SetHealth(float NewHealth);

public:	
	// Sets default values for this component's properties
	URP_HealthComponent();

	UFUNCTION(BlueprintCallable)
	bool IsDead() const {return bIsDead; };

	UFUNCTION(BlueprintCallable, Category="Health")
    void InvulnerableState(bool bNewInvulnerable);

	UFUNCTION()
	void TakingDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;		
};
