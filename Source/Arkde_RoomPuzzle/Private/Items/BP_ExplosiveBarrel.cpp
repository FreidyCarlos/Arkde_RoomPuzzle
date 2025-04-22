// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/BP_ExplosiveBarrel.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Components/RP_HealthComponent.h"
#include "Components/RP_BurnComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"

// Sets default values
ABP_ExplosiveBarrel::ABP_ExplosiveBarrel()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	BarrelMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BarrelMesh"));
	BarrelMesh->SetCollisionProfileName("PhysicsActor");
	RootComponent = BarrelMesh;

	OverlapSphere = CreateDefaultSubobject<USphereComponent>("OverlapSphere");
	OverlapSphere->SetupAttachment(RootComponent);
	OverlapSphere->SetSphereRadius(300.0f);
	OverlapSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	OverlapSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	OverlapSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	HealthComponent = CreateDefaultSubobject<URP_HealthComponent>(TEXT("HealthComponent"));

	BurnDamagePerSecond = 10.0f;
	BurnDurationSeconds = 5.0f;
}

// Called when the game starts or when spawned
void ABP_ExplosiveBarrel::BeginPlay()
{
	Super::BeginPlay();
	
	if (HealthComponent)
	{
		HealthComponent->OnHealthChangeDelegate.AddDynamic(this, &ABP_ExplosiveBarrel::HandleHealthChanged);
	}
}

void ABP_ExplosiveBarrel::HandleHealthChanged(URP_HealthComponent* OwningHealthComp, AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	if (OwningHealthComp)
	{
		Explode();
	}
}

void ABP_ExplosiveBarrel::Explode()
{
	if (ExplosionEffect)
	{
		UGameplayStatics::SpawnEmitterAtLocation(this, ExplosionEffect, GetActorLocation());
	}

	TArray<AActor*> AffectedActors;
	OverlapSphere->GetOverlappingActors(AffectedActors, APawn::StaticClass());

	for (AActor* Actor : AffectedActors)
	{
		if (!Actor) continue;

		URP_BurnComponent* BurnComp = Actor->FindComponentByClass<URP_BurnComponent>();
		if (BurnComp)
		{
			BurnComp->StartBurn(BurnDamagePerSecond, BurnDurationSeconds);
		}
	}

	Destroy();
}

// Called every frame
void ABP_ExplosiveBarrel::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

