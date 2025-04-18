// Fill out your copyright notice in the Description page of Project Settings.

#include "RP_Character.h"
#include "Arkde_RoomPuzzle/Arkde_RoomPuzzle.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/InputComponent.h"
#include "Weapons/RP_Weapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/RP_HealthComponent.h"


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
	MeleeSocketName = "SCK_Melee";
	MeleeSocketName2 = "SCK_Melee2";

	MaxComboMultiplier = 4.0f;
	CurrentComboMultiplier = 1.0f;

	FPSCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FPS_CameraComponent"));
	FPSCameraComponent->bUsePawnControlRotation = true;
	FPSCameraComponent->SetupAttachment(GetMesh(), FPSCameraSocketName);

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SprinArmComponent"));
	SpringArmComponent->bUsePawnControlRotation = true;
	SpringArmComponent->SetupAttachment(RootComponent);

	TPSCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TPS_CameraComponent"));
	TPSCameraComponent->SetupAttachment(SpringArmComponent);

	DashDistance = 2000.0f;
	DashCooldown = 1.0f;
	DashDuration = 0.7f;

	MeleeDetectorComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("MeleeDetectorComponent"));
	MeleeDetectorComponent->SetupAttachment(GetMesh(), MeleeSocketName);
	MeleeDetectorComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	MeleeDetectorComponent->SetCollisionResponseToChannel(COLLISION_ENEMY, ECR_Overlap);
	MeleeDetectorComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	MeleeDetectorComponent2 = CreateDefaultSubobject<UCapsuleComponent>(TEXT("MeleeDetectorComponent2"));
	MeleeDetectorComponent2->SetupAttachment(GetMesh(), MeleeSocketName2);
	MeleeDetectorComponent2->SetCollisionResponseToAllChannels(ECR_Ignore);
	MeleeDetectorComponent2->SetCollisionResponseToChannel(COLLISION_ENEMY, ECR_Overlap);
	MeleeDetectorComponent2->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	MeleeDamage = 10.0f;

	bCanUseWeapon = true;//para disparar aun asi no haya hecho un ataque melee

	HealthComponent = CreateDefaultSubobject<URP_HealthComponent>(TEXT("HealtComponent"));

}

FVector ARP_Character::GetPawnViewLocation() const//CORRIGE EL INICIO DEL LINETRACE VISUAL EN UNREAL
{
	if (IsValid(FPSCameraComponent) && bUseFirstPersonView)
	{
		return FPSCameraComponent->GetComponentLocation();
	}

	if (IsValid(TPSCameraComponent) && !bUseFirstPersonView)
	{
		return TPSCameraComponent->GetComponentLocation();
	}

	return Super::GetPawnViewLocation();
}

// Called when the game starts or when spawned
void ARP_Character::BeginPlay()
{
	Super::BeginPlay();
	InitializeReferences();
	CreateInitialWeapon();
	MeleeDetectorComponent->OnComponentBeginOverlap.AddDynamic(this, &ARP_Character::MakeMeleeDamage);
	MeleeDetectorComponent2->OnComponentBeginOverlap.AddDynamic(this, &ARP_Character::MakeMeleeDamage);
}

void ARP_Character::InitializeReferences()
{
	if (IsValid(GetMesh()))
	{
		MyAnimInstance = GetMesh()->GetAnimInstance();
	}
}

// Called every frame
void ARP_Character::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ARP_Character::CreateInitialWeapon()
{
	if (IsValid(InitialWeaponClass))
	{
		CurrentWeapon = GetWorld()->SpawnActor<ARP_Weapon>(InitialWeaponClass, GetActorLocation(), GetActorRotation());
		if (IsValid(CurrentWeapon))
		{
			CurrentWeapon->SetCharacterOwner(this);
			CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		}
	}
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

	GetWorldTimerManager().SetTimer(DashTimerHandle, this, &ARP_Character::StopDash, DashDuration, false);
	GetWorldTimerManager().SetTimer(DashCooldownTimerHandle, this, &ARP_Character::ResetDash, DashCooldown, false);
}

void ARP_Character::StopDash()
{
	bIsDashing = false;
}

void ARP_Character::ResetDash()
{
	bCanDash = true;
}

void ARP_Character::StartWeaponAction()
{
	if (!bCanUseWeapon)
	{
		return;
	}

	if (IsValid(CurrentWeapon))
	{
		CurrentWeapon->StartAction();
	}
}

void ARP_Character::StopWeaponAction()
{
	if (!bCanUseWeapon)
	{
		return;
	}

	if (IsValid(CurrentWeapon))
	{
		CurrentWeapon->StopAction();
	}
}

void ARP_Character::StartMelee()
{
	if (bIsDoingMelee && !bCanMakeCombos)
	{
		return;
	}

	if (bCanMakeCombos)
	{
		if (bIsDoingMelee)
		{
			if (bIsComboEnable)
			{
				if (CurrentComboMultiplier < MaxComboMultiplier)
				{
					CurrentComboMultiplier++;
					SetComboEnable(false);
				}
				else {
					return;
				}
			}
			else {
				return;
			}
		}
	}

	if (IsValid(MyAnimInstance))
	{
		if (CurrentComboMultiplier == MaxComboMultiplier && IsValid(MeleeMontage3))
		{
			MyAnimInstance->Montage_Play(MeleeMontage3);
		}
		else if (CurrentComboMultiplier >= 2 && IsValid(MeleeMontage2))
		{
			MyAnimInstance->Montage_Play(MeleeMontage2);
		}else if (IsValid(MeleeMontage))
		{
			MyAnimInstance->Montage_Play(MeleeMontage);
		}
	}

	SetMeleeState(true);
}

void ARP_Character::StopMelee()
{
	
}

void ARP_Character::MakeMeleeDamage(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (IsValid(OtherActor))
	{
		UGameplayStatics::ApplyPointDamage(OtherActor, MeleeDamage * CurrentComboMultiplier, SweepResult.Location, SweepResult, GetInstigatorController(), this, nullptr);
	}
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

	PlayerInputComponent->BindAction("WeaponAction", IE_Pressed, this, &ARP_Character::StartWeaponAction);
	PlayerInputComponent->BindAction("WeaponAction", IE_Released, this, &ARP_Character::StopWeaponAction);

	PlayerInputComponent->BindAction("Melee", IE_Pressed, this, &ARP_Character::StartMelee);
	PlayerInputComponent->BindAction("Melee", IE_Released, this, &ARP_Character::StopMelee);
}

void ARP_Character::AddKey(FName NewKey)
{
	DoorKeys.Add(NewKey);
}

bool ARP_Character::HasKey(FName KeyTag)
{
	return DoorKeys.Contains(KeyTag);
}

void ARP_Character::SetMeleeDetectorCollision(ECollisionEnabled::Type NewCollisionState)
{
	MeleeDetectorComponent->SetCollisionEnabled(NewCollisionState);
	MeleeDetectorComponent2->SetCollisionEnabled(NewCollisionState);
}

void ARP_Character::SetMeleeState(bool NewState)
{
	bIsDoingMelee = NewState;
	bCanUseWeapon = !NewState;
}

void ARP_Character::SetComboEnable(bool NewState)
{
	bIsComboEnable = NewState;
}

void ARP_Character::ResetCombo()
{
	SetComboEnable(false);
	CurrentComboMultiplier = 1.0f;
}
