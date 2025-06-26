// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/RP_Enemy.h"
#include "RP_Character.h"
#include "Weapons/RP_Rifle.h"
#include "Weapons/RP_Projectile.h"
#include "Weapons/RP_GrenadeLauncher.h"
#include "Components/RP_HealthComponent.h"
#include "Items/RP_Item.h"
#include "AIModule/Classes/Perception/AISense_Damage.h"
#include "Enemy/Controller/RP_AIController.h"
#include "Enemy/RP_EnemySpawner.h"

ARP_Enemy::ARP_Enemy()
{
	bLoopPath = false;
	DirectionIndex = 1.0;
	WaitingTimeOnPathPoint = 1.0f;
	XPValue = 20.0f;
	LootProbability = 100.0f;
}

void ARP_Enemy::BeginPlay()
{
	Super::BeginPlay();

	MyAiController = Cast<ARP_AIController>(GetController());

	HealthComponent->OnHealthChangeDelegate.AddDynamic(this, &ARP_Enemy::HealthChange);
	HealthComponent->OnDeadDelegate.AddDynamic(this, &ARP_Enemy::GiveXP);
}

void ARP_Enemy::GiveXP(AActor* DamageCauser)
{
	ARP_Character* PossiblePlayer = Cast<ARP_Character>(DamageCauser);
	if (IsValid(PossiblePlayer) && PossiblePlayer->GetCharacterType() == ERP_CharacterType::CharacterType_Player)
	{
		PossiblePlayer->GainUltimateXP(XPValue);

		TrySpawnLoot();
	}

	ARP_Rifle* PossibleRifle = Cast<ARP_Rifle>(DamageCauser);
	if (IsValid(PossibleRifle))
	{
		ARP_Character* RifleOwner = Cast<ARP_Character>(PossibleRifle->GetOwner());

		if (IsValid(RifleOwner) && RifleOwner->GetCharacterType() == ERP_CharacterType::CharacterType_Player)
		{
			RifleOwner->GainUltimateXP(XPValue);

			TrySpawnLoot();
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

				TrySpawnLoot();
			}
		}
	}

	BP_GiveXP(DamageCauser);
}

bool ARP_Enemy::TrySpawnLoot()
{
	if (!IsValid(LootItemClass))
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

	return false;
}

void ARP_Enemy::HealthChange(URP_HealthComponent* CurrentHealthComponent, AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	if (!IsValid(MyAiController))
	{
		return;
	}
	if (CurrentHealthComponent->IsDead())
	{
		if (IsValid(MySpawner))
		{
			MySpawner->NotifyEnemyDead();
		}

		MyAiController->UnPossess();
	}
	else
	{
		ARP_Rifle* Rifle = Cast<ARP_Rifle>(DamageCauser);
		if (IsValid(Rifle))
		{
			AActor* RifleOwner = Rifle->GetOwner();
			MyAiController->SetReceiveDamage(true);
			UAISense_Damage::ReportDamageEvent(GetWorld(), this, RifleOwner, Damage, RifleOwner->GetActorLocation(), FVector::ZeroVector);
		}
	}
}
