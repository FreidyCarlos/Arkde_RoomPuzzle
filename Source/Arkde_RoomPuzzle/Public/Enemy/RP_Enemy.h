// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RP_Character.h"
#include "RP_Enemy.generated.h"

class ARP_PathActor;
class ARP_Item;
class ARP_AIController;
class ARP_EnemySpawner;
class UWidgetComponent;
class URP_EnemyHealthBar;

/**
 * 
 */
UCLASS()
class ARKDE_ROOMPUZZLE_API ARP_Enemy : public ARP_Character
{
	GENERATED_BODY()
	
public:

	ARP_Enemy();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Navigation Path")
	ARP_PathActor* MyPath;

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UWidgetComponent* WidgethHealthBarComponent;

protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI|Navigation Path")
	bool bLoopPath;

	UPROPERTY(BlueprintReadOnly, Category = "UI")
	bool bIsShowingHealthBar;

	UPROPERTY(BlueprintReadOnly, Category = "AI")
	bool bIsAlert;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI|Navigation Path")
	int DirectionIndex;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI|Navigation Path")
	float WaitingTimeOnPathPoint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Ultimate XP")
	float XPValue;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Loot System")
	float LootProbability;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Loot System")
	TSubclassOf<ARP_Item> LootItemClass;

	UPROPERTY(BlueprintReadOnly, Category = "AI|Controller")
	ARP_AIController* MyAiController;

	UPROPERTY(BlueprintReadOnly, Category = "Spawner")
	ARP_EnemySpawner* MySpawner;

	UPROPERTY(BlueprintReadOnly, Category = "UI")
	URP_EnemyHealthBar* EnemyHealthBar;

	FTimerHandle TimerHandle_HideHealthBar;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

protected:

	UFUNCTION()
	void GiveXP(AActor* DamageCauser);

	UFUNCTION(BlueprintImplementableEvent)
	void BP_GiveXP(AActor* DamageCauser);

	bool TrySpawnLoot();

	UFUNCTION()
	void HealthChange(URP_HealthComponent* CurrentHealthComponent, AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	UFUNCTION()
	void HandleHealthUpdate(float CurrentHealth, float MaxHealth);

public:

	bool GetLoopPath() { return bLoopPath; };

	int GetDirectionIndex() { return DirectionIndex; };

	float GetWaitingTime() { return WaitingTimeOnPathPoint; };

	void SetSpawner(ARP_EnemySpawner* NewSpawner) { MySpawner = NewSpawner; };

	void ShowHealthBar();

	void HideHealthBar();

	bool IsAlert() { return bIsAlert; };

	void SetAlert(bool bValue);
};
