// Fill out your copyright notice in the Description page of Project Settings.

#include "RP_Character.h"
# include "Camera/CameraComponent.h"
# include "GameFramework/SpringArmComponent.h"
# include "GameFramework/Actor.h"


// Sets default values
ARP_Character::ARP_Character()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bUseFirstPersonView = true;

	PrimaryActorTick.bCanEverTick = true;//weas del dash
	bUseFirstPersonView = true;
	bCanDash = true;

	FPSCameraSocketName = "SCK_Camera";

	FPSCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FPS_CameraComponent"));
	FPSCameraComponent->bUsePawnControlRotation = true;
	FPSCameraComponent->SetupAttachment(GetMesh(), FPSCameraSocketName);

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SprinArmComponent"));
	SpringArmComponent->bUsePawnControlRotation = true;
	SpringArmComponent->SetupAttachment(RootComponent);

	TPSCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TPS_CameraComponent"));
	TPSCameraComponent->SetupAttachment(SpringArmComponent);

	DashDistance = 3000.0f;
	DashCooldown = 1.0f;
	DashDuration = 0.7f;
}

// Called when the game starts or when spawned
void ARP_Character::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ARP_Character::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ARP_Character::MoveForward(float value)
{
	AddMovementInput(GetActorForwardVector() * value);
}

void ARP_Character::MoveRight(float value)
{
	AddMovementInput(GetActorRightVector() * value);
}

void ARP_Character::AddControllerPitchInput(float value)
{
	Super::AddControllerPitchInput(bIsLookInversion? -value : value);
}

void ARP_Character::Jump()
{
	Super::Jump();
}

void ARP_Character::StopJumping()
{
	Super::StopJumping();
}

void ARP_Character::StartDash()
{
	if (!bCanDash) return;

	if (GetLastMovementInputVector().IsNearlyZero())
	{
		return; // Si no hay movimiento, no permite hacer dash
	}

	bCanDash = false;
	bIsDashing = true; // Se activa el Dash

	FVector DashDirection = GetActorForwardVector();

	LaunchCharacter(DashDirection * DashDistance, true, true);

	if (DashMontage)
	{
		PlayAnimMontage(DashMontage);
	}

	GetWorldTimerManager().SetTimer(DashTimerHandle, this, &ARP_Character::StopDash, DashDuration, false);
	GetWorldTimerManager().SetTimer(DashCooldownTimerHandle, this, &ARP_Character::ResetDash, DashCooldown, false);
}

void ARP_Character::StopDash()
{
	if (DashMontage)
	{
		StopAnimMontage(DashMontage);
	}
	bIsDashing = false;
}

void ARP_Character::ResetDash()
{
	bCanDash = true;
}

// Called to bind functionality to input
void ARP_Character::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &ARP_Character::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ARP_Character::MoveRight);

	PlayerInputComponent->BindAxis("LookUp", this, &ARP_Character::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookRight", this, &ACharacter::AddControllerYawInput);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ARP_Character::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ARP_Character::StopJumping);

	PlayerInputComponent->BindAction("Dash", IE_Pressed, this, &ARP_Character::StartDash);

}