// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/RP_Bot.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "RP_Character.h"
#include "NavigationSystem/Public/NavigationSystem.h"
#include "NavigationSystem/Public/NavigationPath.h"
#include "DrawDebugHelpers.h"
#include "Components/RP_HealthComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Components/SphereComponent.h"
#include "Particles/ParticleSystem.h"
#include "Weapons/RP_Rifle.h"
#include "Weapons/RP_Projectile.h"
#include "Weapons/RP_GrenadeLauncher.h"
#include "Items/RP_Item.h"
#include "Enemy/RP_BotSpawner.h"
#include "Items/RP_SpawnDesactivator.h"

// Sets default values
ARP_Bot::ARP_Bot()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BotMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BotMeshComponent"));
	BotMeshComponent->SetCanEverAffectNavigation(false);
	BotMeshComponent->SetSimulatePhysics(true);
	RootComponent = BotMeshComponent;

	HealthComponent = CreateDefaultSubobject<URP_HealthComponent>(TEXT("HealthComponent"));

	SelfDestructionDetectorComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SelfDestructionDetector"));
	SelfDestructionDetectorComponent->SetupAttachment(RootComponent);
	SelfDestructionDetectorComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	SelfDestructionDetectorComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	SelfDestructionDetectorComponent->SetSphereRadius(150.0f);

	MinDistanceToTarget = 100.0f;
	ForceMagnitude = 500.0f;
	ExplosionDamage = 100.0f;
	ExplosionRadius = 50.0f;

	XPValue = 20.0f;

	LootProbability = 70.0f;

	DesactivatorProbability = 25.0f;
}

// Called when the game starts or when spawned
void ARP_Bot::BeginPlay()
{
	Super::BeginPlay();
	
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (IsValid(PlayerPawn))
	{
		PlayerCharacter = Cast<ARP_Character>(PlayerPawn);
	}

	HealthComponent->OnHealthChangeDelegate.AddDynamic(this, &ARP_Bot::TakingDamage);
	HealthComponent->OnDeadDelegate.AddDynamic(this, &ARP_Bot::GiveXP);

	SelfDestructionDetectorComponent->OnComponentBeginOverlap.AddDynamic(this, &ARP_Bot::StartCountDown);

	BotMaterial = BotMeshComponent->CreateAndSetMaterialInstanceDynamicFromMaterial(0, BotMeshComponent->GetMaterial(0));

	NextPathPoint = GetNextPathPoint();
}

FVector ARP_Bot::GetNextPathPoint()
{
	if (!IsValid(PlayerCharacter))
	{
		return GetActorLocation();
	}

	UNavigationPath* NavigationPath = UNavigationSystemV1::FindPathToActorSynchronously(GetWorld(), GetActorLocation(), PlayerCharacter);
	if (NavigationPath->PathPoints.Num() > 1)
	{
		return NavigationPath->PathPoints[1];
	}

	//if navigation points are less or equal than 1
	return GetActorLocation();
}

void ARP_Bot::TakingDamage(URP_HealthComponent* CurrentHealthComponent, AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	if (IsValid(BotMaterial))
	{

		BotMaterial->SetScalarParameterValue("Pulse", GetWorld()->TimeSeconds);
	}

	if (CurrentHealthComponent->IsDead())
	{
		if (IsValid(DamageCauser))
		{
			ARP_Rifle* Rifle = Cast<ARP_Rifle>(DamageCauser);
			if (IsValid(Rifle))
			{
				ARP_Character* RifleOwner = Cast<ARP_Character>(Rifle->GetOwner());
				if (IsValid(RifleOwner) && RifleOwner->GetCharacterType() == ERP_CharacterType::CharacterType_Player)
				{
					TrySpawnLoot();
				}
			}
			else
			{
				ARP_Projectile* Projectile = Cast<ARP_Projectile>(DamageCauser);
				if (IsValid(Projectile))
				{
					ARP_GrenadeLauncher* ProjectileCaster = Cast<ARP_GrenadeLauncher>(Projectile->GetOwner());

					if (IsValid(ProjectileCaster))
					{
						ARP_Character* GrenadeLauncherOwner = Cast<ARP_Character>(ProjectileCaster->GetOwner());

						if (IsValid(GrenadeLauncherOwner) && GrenadeLauncherOwner->GetCharacterType() == ERP_CharacterType::CharacterType_Player)
						{
							GrenadeLauncherOwner->GainUltimateXP(XPValue);

							TrySpawnLoot();
						}
					}
				}
			}
		}
		SelfDestruction();
	}
}

void ARP_Bot::SelfDestruction()
{
	if (bIsExploted)
	{
		return;
	}

	bIsExploted = true;

	if (IsValid(ExplosionEffect))
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionEffect, GetActorLocation());
	}

	TArray<AActor*> IgnoredActors;
	IgnoredActors.Add(this);

	UGameplayStatics::ApplyRadialDamage(GetWorld(), ExplosionDamage, GetActorLocation(), ExplosionRadius, nullptr, IgnoredActors, this, GetInstigatorController(), true);

	if (bDebug)
	{
		DrawDebugSphere(GetWorld(), GetActorLocation(), ExplosionRadius, 20, FColor::Red, true, 5.0f, 0, 2.0f);
	}

	if (IsValid(MySpawner))
	{
		MySpawner->NotifyBotDead();
	}

	Destroy();
}

void ARP_Bot::StartCountDown(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (bIsStartingCountDown)
	{
		return;
	}

	if (OtherActor == PlayerCharacter)
	{
		bIsStartingCountDown = true;

		GetWorld()->GetTimerManager().SetTimer(TimerHandleSelfDamage, this, &ARP_Bot::SelfDamage, 0.5f, true);
	}
}

void ARP_Bot::SelfDamage()
{
	UGameplayStatics::ApplyDamage(this, 20.0f, GetInstigatorController(), nullptr, nullptr);
}

void ARP_Bot::GiveXP(AActor* DamageCauser)
{
	ARP_Character* PossiblePlayer = Cast<ARP_Character>(DamageCauser);
	if (IsValid(PossiblePlayer) && PossiblePlayer->GetCharacterType() == ERP_CharacterType::CharacterType_Player)
	{
		PossiblePlayer->GainUltimateXP(XPValue);
	}

	ARP_Rifle* PossibleRifle = Cast<ARP_Rifle>(DamageCauser);
	if (IsValid(PossibleRifle))
	{
		ARP_Character* RifleOwner = Cast<ARP_Character>(PossibleRifle->GetOwner());

		if (IsValid(RifleOwner) && RifleOwner->GetCharacterType() == ERP_CharacterType::CharacterType_Player)
		{
			RifleOwner->GainUltimateXP(XPValue);
		}
	}

	ARP_Projectile* PossibleProjectile = Cast<ARP_Projectile>(DamageCauser);
	if (IsValid(PossibleProjectile))
	{
		ARP_GrenadeLauncher* ProjectileCaster = Cast<ARP_GrenadeLauncher>(PossibleProjectile->GetOwner());


		if (IsValid(ProjectileCaster))
		{
			ARP_Character* GrenadeLauncherOwner = Cast<ARP_Character>(ProjectileCaster->GetOwner());

			if (IsValid(GrenadeLauncherOwner) && GrenadeLauncherOwner->GetCharacterType() == ERP_CharacterType::CharacterType_Player)
			{
				GrenadeLauncherOwner->GainUltimateXP(XPValue);
			}
		}
	}

	BP_GiveXP(DamageCauser);
}

bool ARP_Bot::TrySpawnLoot()
{
	if (!IsValid(LootItemClass) || !IsValid(DesactivatorClass))
	{
		return false;
	}

	float SelectorProobability = FMath::RandRange(0.0f, 100.0f);

	if (SelectorProobability <= LootProbability)
	{
		FActorSpawnParameters SpawnParameter;
		SpawnParameter.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		GetWorld()->SpawnActor<ARP_Item>(LootItemClass, GetActorLocation(), FRotator::ZeroRotator, SpawnParameter);
	}
	
	if (SelectorProobability <= DesactivatorProbability && MySpawner->IsSpawnActive())
	{
		FActorSpawnParameters SpawnParameter;
		SpawnParameter.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		ARP_SpawnDesactivator* Key = GetWorld()->SpawnActor<ARP_SpawnDesactivator>(DesactivatorClass, GetActorLocation(), FRotator::ZeroRotator, SpawnParameter);
		if (IsValid(Key))
		{
			Key->SetOwningSpawner(MySpawner);
			Key->SetIsPick(true);
		}
	}

	return false;
}

// Called every frame
void ARP_Bot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	float DistanceToTarget = (GetActorLocation() - NextPathPoint).Size();
	if (DistanceToTarget <= MinDistanceToTarget)
	{
		NextPathPoint = GetNextPathPoint();

	}
	else {
		FVector ForceDirection = NextPathPoint - GetActorLocation();
		ForceDirection.Normalize();
		ForceDirection *= ForceMagnitude;

		BotMeshComponent->AddForce(ForceDirection, NAME_None, true);
	}

	if (bDebug)
	{
		DrawDebugSphere(GetWorld(), NextPathPoint, 30.0f, 15, FColor::Purple, false, 0.0f, 0, 1.0f);
	}
}

