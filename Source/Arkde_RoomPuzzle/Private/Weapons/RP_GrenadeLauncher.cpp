// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/RP_GrenadeLauncher.h"
#include "Weapons/RP_Projectile.h"
#include "GameFramework/Character.h"
#include "Components/SkeletalMeshComponent.h"
#include "RP_Character.h"

ARP_GrenadeLauncher::ARP_GrenadeLauncher() 
{
	MuzzleSocketName = "SCK_Muzzle";
}

void ARP_GrenadeLauncher::StartAction()
{
	Super::StartAction();

    if (IsValid(CurrentOwnerCharacter))
    {
        FVector EyeLocation;
        FRotator EyeRotation;

        CurrentOwnerCharacter->GetActorEyesViewPoint(EyeLocation, EyeRotation);

        FVector ShotDirection = EyeRotation.Vector();

        USkeletalMeshComponent* CharacterMeshComponent = CurrentOwnerCharacter->GetMesh();
        if (IsValid(CharacterMeshComponent))
        {
            FVector MuzzleSocketLocation = CharacterMeshComponent->GetSocketLocation(MuzzleSocketName);
            FRotator SpawnRotation = ShotDirection.Rotation();

            ARP_Projectile* CurrentProjectile = GetWorld()->SpawnActor<ARP_Projectile>(ProjectileClass, MuzzleSocketLocation, SpawnRotation);
        }
    }
}

void ARP_GrenadeLauncher::StopAction()
{
	Super::StopAction();
}
