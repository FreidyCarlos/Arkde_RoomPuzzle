// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/RP_BurnComponent.h"
#include "Components/RP_HealthComponent.h"
#include "TimerManager.h"
#include "GameFramework/Actor.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SkeletalMeshComponent.h"

URP_BurnComponent::URP_BurnComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

// Called when the game starts
void URP_BurnComponent::BeginPlay()
{
	Super::BeginPlay();

	HealthComponent = GetOwner()->FindComponentByClass<URP_HealthComponent>();
}

void URP_BurnComponent::StartBurn(float InDamagePerSecond, float InDurationSeconds)
{
    if (!HealthComponent || InDamagePerSecond <= 0.f || InDurationSeconds <= 0.f)
    {
        return;
    }
 
    GetWorld()->GetTimerManager().ClearTimer(TickTimerHandle);
    if (ActiveEmitter)
    {
        ActiveEmitter->Deactivate();
        ActiveEmitter = nullptr;
    }

    DamagePerSecond = InDamagePerSecond;
    RemainingTicks = FMath::CeilToInt(InDurationSeconds);
    DurationSeconds = InDurationSeconds; 

    if (BurnEmitterTemplate)
    {
        if (USkeletalMeshComponent* Mesh = GetOwner()->FindComponentByClass<USkeletalMeshComponent>())
        {
            ActiveEmitter = UGameplayStatics::SpawnEmitterAttached(BurnEmitterTemplate,Mesh,AttachSocketName,FVector::ZeroVector,FRotator::ZeroRotator,EAttachLocation::KeepRelativeOffset,true);
        }
    }

    OnBurnStarted.Broadcast(GetOwner());
    GetWorld()->GetTimerManager().SetTimer(TickTimerHandle,this,&URP_BurnComponent::HandleBurnTick,1.0f,true,1.0f);
}

void URP_BurnComponent::HandleBurnTick()
{
    if (!HealthComponent) return;

    HealthComponent->TakingDamage(GetOwner(), DamagePerSecond, nullptr, nullptr, nullptr);

    if (--RemainingTicks <= 0)
    {
        if (ActiveEmitter) ActiveEmitter->Deactivate();
        GetWorld()->GetTimerManager().ClearTimer(TickTimerHandle);
        OnBurnEnded.Broadcast(GetOwner());
    }
}

void URP_BurnComponent::HandleBurnEnd()
{
    GetWorld()->GetTimerManager().ClearTimer(TickTimerHandle);
    GetWorld()->GetTimerManager().ClearTimer(EndTimerHandle);

    if (ActiveEmitter)
    {
        ActiveEmitter->Deactivate();
        ActiveEmitter = nullptr;
    }

    OnBurnEnded.Broadcast(GetOwner());
}

