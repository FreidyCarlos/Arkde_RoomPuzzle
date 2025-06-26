// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RP_HealerSpawner.generated.h"

class UBillboardComponent;
class ARP_Healer;

UCLASS()
class ARKDE_ROOMPUZZLE_API ARP_HealerSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARP_HealerSpawner();

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UBillboardComponent* SpawnerPathBillboardComponent;

protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawner")
	bool bIsActive;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawner", meta = (UIMin = 1.0, ClampMin = 1.0))
	int MaxBotsCounter;

	UPROPERTY(BlueprintReadOnly, Category = "Spawner")
	int CurrentBotsCounter;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawner", meta = (UIMin = 0.1, ClampMin = 0.1))
	float TimeToSpawn;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, BlueprintReadOnly, Category = "Spawner", meta = (MakeEditWidget = true))
	TArray<FVector> SpawnPoints;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spawner")
	TSubclassOf<ARP_Healer> BotClass;

	FTimerHandle TimerHandleSpawnBot;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void SpawnBot();

	FVector GetSpawnPoint();

public:

	void NotifyBotDead();
};
