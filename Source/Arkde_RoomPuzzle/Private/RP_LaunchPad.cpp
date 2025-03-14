// Fill out your copyright notice in the Description page of Project Settings.


#include "RP_LaunchPad.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/Actor.h"

// Sets default values
ARP_LaunchPad::ARP_LaunchPad()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	RootComponent = CollisionBox;
	CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &ARP_LaunchPad::OnLaunchPadActivated);

	PlatformMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlatformMesh"));
	PlatformMesh->SetupAttachment(RootComponent);

}

// Called when the game starts or when spawned
void ARP_LaunchPad::BeginPlay()
{
	Super::BeginPlay();
	
}

void ARP_LaunchPad::OnLaunchPadActivated(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!bIsEnabled)
	{
		return;
	}

	if (IsValid(OtherActor))
	{
		ACharacter* OverlappedCharacter = Cast<ACharacter>(OtherActor);
		if (IsValid(OverlappedCharacter))
		{
			OverlappedCharacter->LaunchCharacter(LaunchVelocity, true, true);
		}
	}
}

// Called every frame
void ARP_LaunchPad::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ARP_LaunchPad::SetLaunchPadEnabled(bool bNewState)
{
	bIsEnabled = bNewState;
}

