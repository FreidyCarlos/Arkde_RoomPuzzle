// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "RP_Healer.generated.h"

class UStaticMeshComponent;
class URP_HealthComponent;
class ARP_Character;
class UMaterialInstanceDynamic;
class UParticleSystem;
class USphereComponent;
class ARP_Character;
class ARP_Item;
class ARP_HealerSpawner;

UENUM(BlueprintType)
enum class EHealerState : uint8
{
    Patrolling    UMETA(DisplayName = "Patrolling"),
    MovingToTarget UMETA(DisplayName = "MovingToTarget"),
    Healing       UMETA(DisplayName = "Healing"),
	ReturningToPatrol UMETA(DisplayName = "ReturningToPatrol")
};

UCLASS()
class ARKDE_ROOMPUZZLE_API ARP_Healer : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ARP_Healer();

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* BotMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	URP_HealthComponent* HealthComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    USphereComponent* HealingSphere;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bot Effect | Dead")
	UParticleSystem* DeadEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bot Effect | Healing")
	UParticleSystem* HealingEffect;

	UParticleSystemComponent* ActiveHealingEmitterComponent;

protected:

	UPROPERTY(BlueprintReadOnly, Category = "Bot Destruction")
	bool bIsDead;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Debug")
	bool bDebug;

	bool bIsPatrolling;

	float PatrolAngle;

    UPROPERTY(EditDefaultsOnly, Category = "Patrol")
    float PatrolRadius;

    UPROPERTY(EditDefaultsOnly, Category = "Patrol")
    float PatrolSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Healing")
    float HealRadius;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Healing")
    float HealAmountPerTick;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Healing")
    float HealInterval;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Healing", meta = (ClampMin = "0.0"))
	float MinHealDistance = 200.0f;

	UPROPERTY(EditAnywhere, Category="Movement")
	float TargetMoveSpeed = 500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bot Ultimate XP")
	float XPValue;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Loot System")
	float LootProbability;

	UMaterialInstanceDynamic* HealerMaterial;

    FVector PatrolCenter;

	FVector LastPatrolLocation;

	URP_HealthComponent* CurrentTargetHealthComp;

	EHealerState HealerState;

	TArray<URP_HealthComponent*> DamagedTargets;

	FTimerHandle HealTimerHandle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Loot System")
	TSubclassOf<ARP_Item> LootItemClass;

	UPROPERTY(BlueprintReadOnly, Category = "Spawner")
	ARP_HealerSpawner* MySpawner;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void ProcessCurrentOverlaps();

	bool IsInHealingSphere(URP_HealthComponent* HealthComp) const;

	UFUNCTION()
	void TakingDamage(URP_HealthComponent* CurrentHealthComponent, AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	void Destruction();

	UFUNCTION()
    void OnHealingSphereBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

    UFUNCTION()
    void OnHealingSphereEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void OnTargetHealthChanged(URP_HealthComponent* HealthComp, AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser);

	void BeginMoveToTarget();

	void SelectNextOrReturn();

	void UpdatePatrol(float DeltaTime);

	void TrySelectNewTarget();

	void StartHealing();

	void StopHealing();

	void HealTick();

	URP_HealthComponent* FindNearestDamaged();

	void MoveTowards(const FVector& TargetLocation, float DeltaTime);

	UFUNCTION()
	void GiveXP(AActor* DamageCauser);

	UFUNCTION(BlueprintImplementableEvent)
	void BP_GiveXP(AActor* DamageCauser);

	bool TrySpawnLoot();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SetSpawner(ARP_HealerSpawner* NewSpawner) { MySpawner = NewSpawner; };
};
