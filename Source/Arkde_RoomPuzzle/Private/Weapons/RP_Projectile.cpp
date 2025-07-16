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
#include "Sound/SoundCue.h"


// Sets default values
ARP_Projectile::ARP_Projectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ProjectileCollision = CreateDefaultSubobject<USphereComponent>(TEXT("ProjectileCollision"));
	ProjectileCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	ProjectileCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	RootComponent = ProjectileCollision;

	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMesh"));
	ProjectileMesh->SetupAttachment(ProjectileCollision);
	ProjectileMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComp"));
	ProjectileMovementComponent->InitialSpeed = 3000.0f;
	ProjectileMovementComponent->MaxSpeed = 3000.0f;
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
	ProjectileMovementComponent->bShouldBounce = false;

	ExplosionDelay = 2.5f;
	ExplosionRadius = 130.0f;
	ExplosionDamage = 70.0f;

	ProjectileCollision->OnComponentHit.AddDynamic(this, &ARP_Projectile::OnHit);  // Escuchar el evento de colisión
}

// Called when the game starts or when spawned
void ARP_Projectile::BeginPlay()
{
	Super::BeginPlay();

	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (IsValid(PlayerPawn))
	{
		PlayerCharacter = Cast<ARP_Character>(PlayerPawn);
	}

	GetWorld()->GetTimerManager().SetTimer(ExplosionTimerHandle, this, &ARP_Projectile::Explode, ExplosionDelay, false);//esto es para que explote aun asi no colisione con nada
}

void ARP_Projectile::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (IsValid(OtherActor))
	{
		if (OtherActor != PlayerCharacter)
		{
			if (OtherComp->GetCollisionObjectType() == ECC_Pawn)
			{
				Explode();
			}
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

	if (bIsDebuging)
	{
		FVector DamageLocation = GetActorLocation();
		DrawDebugString(GetWorld(), DamageLocation + FVector(0, 0, 100), FString::Printf(TEXT("Damage: %.0f"), ExplosionDamage), nullptr, FColor::Orange, 3.0f);
		DrawDebugSphere(GetWorld(), GetActorLocation(), ExplosionRadius, 12, FColor::Red, false, 3.0f);
	}
	PlaySound(ExplosionAudio);

	Destroy();
}

// Called every frame
void ARP_Projectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}	

void ARP_Projectile::PlaySound(USoundCue* SoundCue, bool bIs3D /*= false*/, FVector SoundLocation /*= FVector::ZeroVector*/)
{
	if (!IsValid(SoundCue))
	{
		return;
	}

	if (bIs3D)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), SoundCue, SoundLocation);
	}
	else
	{
		UGameplayStatics::PlaySound2D(GetWorld(), SoundCue);
	}
}

