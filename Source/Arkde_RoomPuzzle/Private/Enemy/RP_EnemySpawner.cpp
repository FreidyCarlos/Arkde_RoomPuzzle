// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/RP_EnemySpawner.h"
#include "Components/BillboardComponent.h"
#include "Enemy/RP_Enemy.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
ARP_EnemySpawner::ARP_EnemySpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpawnerPathBillboardComponent = CreateDefaultSubobject<UBillboardComponent>(TEXT("PathBillboard"));
	RootComponent = SpawnerPathBillboardComponent;

	bIsActive = true;
	MaxBotsCounter = 4;
	TimeToSpawn = 1.0f;

	NextSpawnIndex = 0;
}

// Called when the game starts or when spawned
void ARP_EnemySpawner::BeginPlay()
{
	Super::BeginPlay();
	
	GetWorld()->GetTimerManager().SetTimer(TimerHandleSpawnBot, this, &ARP_EnemySpawner::SpawnEnemy, TimeToSpawn, true);
}

void ARP_EnemySpawner::SpawnEnemy()
{
	if (!bIsActive)
	{
		return;
	}

	if (CurrentEnemyCounter >= MaxBotsCounter)
	{
		return;
	}

	if (IsValid(EnemyClass))
	{
		FVector LocalSpawnPoint = GetSpawnPoint();
		FVector SpawnPoint = UKismetMathLibrary::TransformLocation(GetActorTransform(), LocalSpawnPoint);

		FTransform EnemyTransform = FTransform(FRotator::ZeroRotator, SpawnPoint);

		ARP_Enemy* NewEnemy = GetWorld()->SpawnActorDeferred<ARP_Enemy>(EnemyClass, EnemyTransform);

		if (Path)
		{
			NewEnemy->MyPath = Path;
		}

		if (IsValid(NewEnemy))
		{
			NewEnemy->SetSpawner(this);
		}

		NewEnemy->FinishSpawning(EnemyTransform);

		CurrentEnemyCounter++;
	}
}

FVector ARP_EnemySpawner::GetSpawnPoint()
{
	if (SpawnPoints.Num() > 0)
	{
		int32 IndexSelected = NextSpawnIndex;

		NextSpawnIndex = (NextSpawnIndex + 1) % SpawnPoints.Num();

		return SpawnPoints[IndexSelected];
	}
	else
	{
		return GetActorLocation();
	}
}

void ARP_EnemySpawner::NotifyEnemyDead()
{
	CurrentEnemyCounter--;
}
