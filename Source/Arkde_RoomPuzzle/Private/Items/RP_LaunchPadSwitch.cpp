// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/RP_LaunchPadSwitch.h"
#include "RP_LaunchPad.h"
#include "RP_Character.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Items/RP_Item.h"
#include "Engine/Engine.h"

ARP_LaunchPadSwitch::ARP_LaunchPadSwitch()
{
	bIsLaunchPadActive = false;

    ButtonMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ButtonMesh"));
    ButtonMesh->SetupAttachment(MainColliderComponent);
    ButtonMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ARP_LaunchPadSwitch::BeginPlay()
{
    Super::BeginPlay();

    if (MainColliderComponent)
    {
        MainColliderComponent->OnComponentBeginOverlap.AddDynamic(this, &ARP_LaunchPadSwitch::LaunchPadButton);
    }
}

void ARP_LaunchPadSwitch::LaunchPadButton(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (IsValid(OtherActor))
    {
        ARP_Character* OverlappedCharacter = Cast<ARP_Character>(OtherActor);
        if (IsValid(OverlappedCharacter))
        {
            if (LinkedLaunchPad)
            {
                bIsLaunchPadActive = !bIsLaunchPadActive;
                LinkedLaunchPad->SetLaunchPadEnabled(bIsLaunchPadActive);

                //Mostrar mensaje en pantalla según el estado (porque me quedo grande en Blueprint ahhhh)
                if (GEngine)
                {
                    FColor MessageColor = bIsLaunchPadActive ? FColor::Green : FColor::Red;
                    FString MessageText = bIsLaunchPadActive ? TEXT("Plataforma Activada") : TEXT("Plataforma Desactivada");

                    GEngine->AddOnScreenDebugMessage(-1, 2.0f, MessageColor, MessageText);
                }
            }
        }
    }
}
