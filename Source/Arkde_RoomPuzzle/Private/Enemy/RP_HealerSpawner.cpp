// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/RP_HealerSpawner.h"
#include "Components/BillboardComponent.h"
#include "Bot/RP_Healer.h"
#include "Kismet/KismetMathLibrary.h"


// Sets default values
ARP_HealerSpawner::ARP_HealerSpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpawnerPathBillboardComponent = CreateDefaultSubobject<UBillboardComponent>(TEXT("PathBillboard"));
	RootComponent = SpawnerPathBillboardComponent;

	bIsActive = true;
	MaxBotsCounter = 1;
	TimeToSpawn = 5.0f;
}

// Called when the game starts or when spawned
void ARP_HealerSpawner::BeginPlay()
{
	Super::BeginPlay();
	
	GetWorld()->GetTimerManager().SetTimer(TimerHandleSpawnBot, this, &ARP_HealerSpawner::SpawnBot, TimeToSpawn, true);
}

void ARP_HealerSpawner::SpawnBot()
{
	if (!bIsActive)
	{
		return;
	}

	if (CurrentBotsCounter >= MaxBotsCounter)
	{
		return;
	}

	if (IsValid(BotClass))
	{
		FVector LocalSpawnPoint = GetSpawnPoint();
		FVector SpawnPoint = UKismetMathLibrary::TransformLocation(GetActorTransform(), LocalSpawnPoint);

		FTransform BotTransform = FTransform(FRotator::ZeroRotator, SpawnPoint);

		ARP_Healer* NewHealer = GetWorld()->SpawnActorDeferred<ARP_Healer>(BotClass, BotTransform);

		if (IsValid(NewHealer))
		{
			NewHealer->SetSpawner(this);
		}

		NewHealer->FinishSpawning(BotTransform);

		CurrentBotsCounter++;
	}
}

FVector ARP_HealerSpawner::GetSpawnPoint()
{
	if (SpawnPoints.Num() > 0)
	{
		int IndexSelected = FMath::RandRange(0, SpawnPoints.Num() - 1);
		return SpawnPoints[IndexSelected];
	}
	else
	{
		return GetActorLocation();
	}
}

void ARP_HealerSpawner::NotifyBotDead()
{
	CurrentBotsCounter--;
}
