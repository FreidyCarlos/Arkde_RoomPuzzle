// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/RP_Projectile.h"
#include "Arkde_RoomPuzzle/Arkde_RoomPuzzle.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "DrawDebugHelpers.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "RP_Character.h"

// Sets default values
ARP_Projectile::ARP_Projectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ProjectileCollision = CreateDefaultSubobject<USphereComponent>(TEXT("ProjectileCollision"));
	RootComponent = ProjectileCollision;

	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMesh"));
	ProjectileMesh->SetupAttachment(ProjectileCollision);

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComp"));

	ProjectileMovementComponent->InitialSpeed = 3000.0f;
	ProjectileMovementComponent->MaxSpeed = 3000.0f;

	ProjectileCollision->OnComponentHit.AddDynamic(this, &ARP_Projectile::OnHit);  // Escuchar el evento de colisión
}

// Called when the game starts or when spawned
void ARP_Projectile::BeginPlay()
{
	Super::BeginPlay();

	GetWorld()->GetTimerManager().SetTimer(ExplosionTimerHandle, this, &ARP_Projectile::Explode, ExplosionDelay, false);//esto es para que explote aun asi no colisione con nada
}

void ARP_Projectile::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor && OtherActor != this)
	{
		if (OtherComp->GetCollisionObjectType() == COLLISION_ENEMY)
		{
			Explode();
		}
	}
}

void ARP_Projectile::Explode()
{
	FVector ImpactPoint = GetActorLocation();
	FRotator ImpactRotation = FRotator::ZeroRotator;

	if (ImpactEffect != nullptr)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactEffect, ImpactPoint, ImpactRotation);
	}

	DrawDebugSphere(GetWorld(), GetActorLocation(), ExplosionRadius, 12, FColor::Red, false, 3.0f);

	TArray<AActor*> IgnoredActors;
	TArray<AActor*> OverlappingActors;

	UKismetSystemLibrary::SphereOverlapActors(GetWorld(), GetActorLocation(), ExplosionRadius, TArray<TEnumAsByte<EObjectTypeQuery>>(), nullptr, IgnoredActors, OverlappingActors);

	for (AActor* Actor : OverlappingActors)
	{
		if (IsValid(Actor))
		{
			UGameplayStatics::ApplyPointDamage(Actor, ExplosionDamage, GetActorForwardVector(), FHitResult(), GetInstigatorController(), this, DamageType);
		}
	}

	//Esto lo piden pero no se ve bonito, lo dejo porque es requisito
	FVector DamageLocation = GetActorLocation();
	DrawDebugString(GetWorld(), DamageLocation + FVector(0, 0, 100), FString::Printf(TEXT("Damage: %.0f"), ExplosionDamage), nullptr, FColor::Orange, 3.0f);

	Destroy();
}

// Called every frame
void ARP_Projectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

