// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RP_BurnComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBurnEvent, AActor*, BurnedActor);

class UParticleSystem;
class UParticleSystemComponent;
class URP_HealthComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ARKDE_ROOMPUZZLE_API URP_BurnComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	URP_BurnComponent();

    UPROPERTY(BlueprintAssignable, Category="Burn|Events")
    FOnBurnEvent OnBurnStarted;

    UPROPERTY(BlueprintAssignable, Category="Burn|Events")
    FOnBurnEvent OnBurnEnded;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	

    UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Settings",meta=(AllowPrivateAccess="true",ClampMin="0.1",UIMin="0.1"))
    float DamagePerSecond;

    UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Settings",meta=(AllowPrivateAccess="true",ClampMin="0.1",UIMin="0.1"))
    float DurationSeconds;

    UPROPERTY(EditAnywhere, Category="Burn|Effects", meta=(Tooltip="Niagara o Cascade emitter"))
    UParticleSystem* BurnEmitterTemplate;

    UPROPERTY(EditAnywhere, Category="Burn|Effects", meta=(Tooltip="Dejar vacío para root"))
    FName AttachSocketName;

    UPROPERTY()
    UParticleSystemComponent* ActiveEmitter;

    int32 RemainingTicks;

    FTimerHandle TickTimerHandle;
    FTimerHandle EndTimerHandle;

    class URP_HealthComponent* HealthComponent;

    UFUNCTION(BlueprintCallable, Category="Control")
    void StartBurn(float InDamagePerSecond, float InDurationSeconds);

    void HandleBurnTick();

    void HandleBurnEnd();
};
