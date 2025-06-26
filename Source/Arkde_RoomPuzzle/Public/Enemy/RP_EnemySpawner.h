// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RP_EnemySpawner.generated.h"

class UBillboardComponent;
class ARP_Enemy;
class ARP_PathActor;


UCLASS()
class ARKDE_ROOMPUZZLE_API ARP_EnemySpawner : public AActor
{
	GENERATED_BODY()
protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UBillboardComponent* SpawnerPathBillboardComponent;

protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawner")
	bool bIsActive;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawner", meta = (UIMin = 1.0, ClampMin = 1.0))
	int MaxBotsCounter;

	UPROPERTY(BlueprintReadOnly, Category = "Spawner")
	int CurrentEnemyCounter;

	int32 NextSpawnIndex;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawner", meta = (UIMin = 0.1, ClampMin = 0.1))
	float TimeToSpawn;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, BlueprintReadOnly, Category = "Spawner", meta = (MakeEditWidget = true))
	TArray<FVector> SpawnPoints;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spawner")
	TSubclassOf<ARP_Enemy> EnemyClass;

	FTimerHandle TimerHandleSpawnBot;

	UPROPERTY(EditDefaultsOnly, Category="AI|Navigation Path")
	TSubclassOf<ARP_PathActor> PathActorClass;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category="AI|Navigation Path")
	ARP_PathActor* Path;

public:	
	// Sets default values for this actor's properties
	ARP_EnemySpawner();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void SpawnEnemy();

	FVector GetSpawnPoint();

public:

	void NotifyEnemyDead();
};
