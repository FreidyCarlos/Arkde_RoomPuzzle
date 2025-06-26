// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/RP_BotSpawner.h"
#include "Components/BillboardComponent.h"
#include "Enemy/RP_Bot.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/BoxComponent.h"
#include "RP_Character.h"

// Sets default values
ARP_BotSpawner::ARP_BotSpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpawnerPathBillboardComponent = CreateDefaultSubobject<UBillboardComponent>(TEXT("PathBillboard"));
	RootComponent = SpawnerPathBillboardComponent;

	ZoneColliderComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("ZoneColliderComponent"));
	ZoneColliderComponent->SetupAttachment(RootComponent);
	ZoneColliderComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	ZoneColliderComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	ZoneColliderComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	bIsActive = false;
	MaxBotsCounter = 3;
	TimeToSpawn = 1.0f;
}

// Called when the game starts or when spawned
void ARP_BotSpawner::BeginPlay()
{
	Super::BeginPlay();

	ZoneColliderComponent->OnComponentBeginOverlap.AddDynamic(this, &ARP_BotSpawner::OnOverlapActor);

	GetWorld()->GetTimerManager().SetTimer(TimerHandleSpawnBot, this, &ARP_BotSpawner::SpawnBot, TimeToSpawn, true);
}

void ARP_BotSpawner::OnOverlapActor(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (IsValid(OtherActor))
	{
		ARP_Character* ActorOn = Cast<ARP_Character>(OtherActor);
		UE_LOG(LogTemp, Warning, TEXT("Casteado!"));
		if (ActorOn->GetCharacterType() == ERP_CharacterType::CharacterType_Player)
		{
			bIsActive = true;
			UE_LOG(LogTemp, Warning, TEXT("Overlaped!"));
			ZoneColliderComponent->SetGenerateOverlapEvents(false);
		}
	}
}

void ARP_BotSpawner::SpawnBot()
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

		ARP_Bot* NewBot = GetWorld()->SpawnActorDeferred<ARP_Bot>(BotClass, BotTransform);

		if (IsValid(NewBot))
		{
			NewBot->SetSpawner(this);
		}

		NewBot->FinishSpawning(BotTransform);

		CurrentBotsCounter++;
	}
}

FVector ARP_BotSpawner::GetSpawnPoint()
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

void ARP_BotSpawner::NotifyBotDead()
{
	CurrentBotsCounter--;
}

void ARP_BotSpawner::SetSpawnActive(bool bActive)
{
	bIsActive = bActive;
}

