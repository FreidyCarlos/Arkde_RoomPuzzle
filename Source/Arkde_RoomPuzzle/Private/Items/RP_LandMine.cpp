// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/RP_LandMine.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/RP_HealthComponent.h"
#include "GameFramework/Character.h"
#include "RP_Character.h"
#include "Components/AudioComponent.h"

// Sets default values
ARP_LandMine::ARP_LandMine()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	MineMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MineMesh"));
	RootComponent = MineMesh;
	MineMesh->SetCollisionProfileName(TEXT("BlockAll"));

	AlertTrigger = CreateDefaultSubobject<USphereComponent>(TEXT("AlertTrigger"));
	AlertTrigger->SetupAttachment(RootComponent);
	AlertTrigger->SetSphereRadius(600.0f);
	AlertTrigger->SetCollisionProfileName(TEXT("Trigger"));

	ExplosionTrigger = CreateDefaultSubobject<USphereComponent>(TEXT("ExplosionTrigger"));
	ExplosionTrigger->SetupAttachment(RootComponent);
	ExplosionTrigger->SetSphereRadius(55.0f);
	ExplosionTrigger->SetCollisionProfileName(TEXT("Trigger"));

	HealthComponent = CreateDefaultSubobject<URP_HealthComponent>(TEXT("HealthComponent"));

	AlertAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("AlertAudioComponent"));
	AlertAudioComponent->SetupAttachment(RootComponent);
	AlertAudioComponent->bAutoActivate = false;

	ExplosionDamage = 110.0f;
	bHasExploded = false;
}

// Called when the game starts or when spawned
void ARP_LandMine::BeginPlay()
{
	Super::BeginPlay();
	
	AlertTrigger->OnComponentBeginOverlap.AddDynamic(this, &ARP_LandMine::OnAlertTriggerOverlap);
	AlertTrigger->OnComponentEndOverlap.AddDynamic(this, &ARP_LandMine::OnAlertTriggerEndOverlap);
	ExplosionTrigger->OnComponentBeginOverlap.AddDynamic(this, &ARP_LandMine::OnExplosionTriggerOverlap);
	HealthComponent->OnHealthChangeDelegate.AddDynamic(this, &ARP_LandMine::OnMineHealthChanged);
}

void ARP_LandMine::OnAlertTriggerOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (bHasExploded) return;

	if (ARP_Character* Player = Cast<ARP_Character>(OtherActor))
	{
		if (!AlertAudioComponent->IsPlaying() && AlertLoopSound)
		{
			AlertAudioComponent->SetSound(AlertLoopSound);
			AlertAudioComponent->Play();
		}
	}
}

void ARP_LandMine::OnAlertTriggerEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (bHasExploded) return;

	if (ARP_Character* Player = Cast<ARP_Character>(OtherActor))
	{
		if (AlertAudioComponent->IsPlaying())
		{
			AlertAudioComponent->Stop();
		}
	}
}

void ARP_LandMine::OnExplosionTriggerOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (bHasExploded) return;

	if (ARP_Character* Player = Cast<ARP_Character>(OtherActor))
	{
		UGameplayStatics::ApplyDamage(Player, ExplosionDamage, nullptr, this, DamageTypeClass);
		Explode();
	}
}

void ARP_LandMine::OnMineHealthChanged(URP_HealthComponent* OwningHealthComp, AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	if (OwningHealthComp->IsDead() && !bHasExploded)
	{
		Explode();
	}
}

void ARP_LandMine::Explode()
{
	bHasExploded = true;

	if (AlertAudioComponent->IsPlaying())
	{
		AlertAudioComponent->Stop();
	}

	if (ExplosionEffect)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionEffect, GetActorLocation());
	}

	if (ExplosionSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ExplosionSound, GetActorLocation());
	}

	Destroy();
}

// Called every frame
void ARP_LandMine::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

