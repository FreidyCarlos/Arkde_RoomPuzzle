// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "RP_Character.generated.h"

class UCameraComponent;
class USpringArmComponent;

UCLASS()
class ARKDE_ROOMPUZZLE_API ARP_Character : public ACharacter
{
	GENERATED_BODY()

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USpringArmComponent* SpringArmComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UCameraComponent* FPSCameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UCameraComponent* TPSCameraComponent;

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Aiming")
	bool bUseFirstPersonView;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Aiming")
	bool bIsLookInversion;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Aiming")
	FName FPSCameraSocketName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Key")
	TArray<FName> DoorKeys;

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dash")
	float DashDistance;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dash")
	float DashCooldown;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dash")
	float DashDuration;

	UPROPERTY(BlueprintReadOnly, Category = "Animation")
	bool bIsDashing;

	FTimerHandle DashTimerHandle;
	FTimerHandle DashCooldownTimerHandle;

	bool bCanDash;

public:
	// Sets default values for this character's properties
	ARP_Character();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void MoveForward(float value);

	void MoveRight(float value);

	virtual void Jump() override;

	virtual void StopJumping() override;

	void StartDash();

	void StopDash();

	void ResetDash();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void AddControllerPitchInput(float value) override;

	void AddKey(FName NewKey);

	bool HasKey(FName KeyTag);
};
