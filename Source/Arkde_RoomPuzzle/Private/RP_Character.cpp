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
#include "Core/RP_GameMode.h"
#include "Components/RP_BurnComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Core/RP_GameInstance.h"


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

	MeleeDetectorComponent3 = CreateDefaultSubobject<UCapsuleComponent>(TEXT("MeleeDetectorComponent3"));
	MeleeDetectorComponent3->SetupAttachment(RootComponent);
	MeleeDetectorComponent3->SetCollisionObjectType(ECC_GameTraceChannel4);  // Canal de colisión de la ultimate
	MeleeDetectorComponent3->SetCollisionResponseToAllChannels(ECR_Ignore);  // Ignorar todas las colisiones por defecto
	MeleeDetectorComponent3->SetCollisionResponseToChannel(COLLISION_ULTIMATE2, ECR_Overlap);
	MeleeDetectorComponent3->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	MeleeDamage = 10.0f;
	

	bCanUseWeapon = true;//para disparar aun asi no haya hecho un ataque melee

	HealthComponent = CreateDefaultSubobject<URP_HealthComponent>(TEXT("HealtComponent"));

	BurnComponent = CreateDefaultSubobject<URP_BurnComponent>(TEXT("BurnComponent"));

	bUltimateWithTick = true;
	MaxUltimateXP = 100.0f;
	MaxUltimateDuration = 10.0f;
	UltimateFrequency = 0.5f;

	UltimateWalkSpeed = 1000.0f;
	UltimatePlayRate = 2.0f;
	PlayRate = 1.0f;
	UltimateShotFrequency = 0.25f;

	MaxUltimateDuration2 = 12.0f;
	UltimateCollisionDamage = 1000.0f;

	MainMenuMapName = "MainMenuMap";

	bUsingPrimaryWeapon = true;
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
	MeleeDetectorComponent3->OnComponentBeginOverlap.AddDynamic(this, &ARP_Character::MakeUltimate2Damage);

	HealthComponent->OnHealthChangeDelegate.AddDynamic(this, &ARP_Character::OnHealthChange);

	NormalWalkSpeed = GetCharacterMovement()->MaxWalkSpeed;
}

void ARP_Character::InitializeReferences()
{
	if (IsValid(GetMesh()))
	{
		MyAnimInstance = GetMesh()->GetAnimInstance();
	}

	GameModeReference = Cast<ARP_GameMode>(GetWorld()->GetAuthGameMode());

	GameInstanceReference = Cast<URP_GameInstance>(GetWorld()->GetGameInstance());
}

// Called every frame
void ARP_Character::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsUsingUltimate && bUltimateWithTick)
	{
		UpdateUltimateDuration(DeltaTime);
	}
}

void ARP_Character::CreateInitialWeapon()
{
	bUsingPrimaryWeapon = true;

	if (InitialWeaponClass)
	{
		CurrentWeapon = GetWorld()->SpawnActor<ARP_Weapon>(InitialWeaponClass, GetActorLocation(), GetActorRotation());
		if (CurrentWeapon)
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

	MyAnimInstance->Montage_Play(DashMontage);

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
	if (!bCanUseWeapon || bIsUsingUltimate2 || bIsChangingWeapon || bIsDashing)
	{
		return;
	}

	if (IsValid(CurrentWeapon))
	{
		MyAnimInstance->Montage_Play(ShootMontage);
		CurrentWeapon->StartAction();

		if (bIsUsingUltimate)
		{
			GetWorld()->GetTimerManager().SetTimer(TimerHandle_AutomaticShoot , CurrentWeapon, &ARP_Weapon::StartAction, UltimateShotFrequency, true);
		}
	}
}

void ARP_Character::StopWeaponAction()
{
	if (!bCanUseWeapon || bIsUsingUltimate2)
	{
		return;
	}

	if (IsValid(CurrentWeapon))
	{
		CurrentWeapon->StopAction();

		if (bIsUsingUltimate)
		{
			GetWorld()->GetTimerManager().ClearTimer(TimerHandle_AutomaticShoot);
		}
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
			MyAnimInstance->Montage_Play(MeleeMontage3, PlayRate);
		}
		else if (CurrentComboMultiplier >= 2 && IsValid(MeleeMontage2))
		{
			MyAnimInstance->Montage_Play(MeleeMontage2, PlayRate);
		}else if (IsValid(MeleeMontage))
		{
			MyAnimInstance->Montage_Play(MeleeMontage, PlayRate);
		}
	}

	SetMeleeState(true);
}

void ARP_Character::StopMelee()
{
	
}

void ARP_Character::StartUltimate()
{
	if (bIsUsingUltimate2)
	{
		return;
	}

	if (bCanUseUltimate && !bIsUsingUltimate)
	{
		CurrentUltimateDuration = MaxUltimateDuration;

		bCanUseUltimate = false;

		if (IsValid(MyAnimInstance) && IsValid(UltimateMontage))
		{
			GetCharacterMovement()->MaxWalkSpeed = 0.0f;
			MyAnimInstance->Montage_Play(UltimateMontage, UltimatePlayRate);
			const float RawLength = UltimateMontage->GetPlayLength();
			const float StartUltimateMontageDuration = RawLength / UltimatePlayRate;
			GetWorld()->GetTimerManager().SetTimer(TimerHandle_BeginUltimateBehaviour, this, &ARP_Character::BeginUltimateBehaviour, StartUltimateMontageDuration, false);
		}
		else {
			BeginUltimateBehaviour();
		}
		BP_StartUltimate();
	}
}

void ARP_Character::StopUltimate() 
{

}

void ARP_Character::StartUltimate2()
{
	if (bIsUsingUltimate)
	{
		return;
	}

	if (bCanUseUltimate && !bIsUsingUltimate2)
	{
		CurrentUltimateDuration2 = MaxUltimateDuration2;

		bCanUseUltimate = false;

		SetInvulnerable(true);

		if (IsValid(MyAnimInstance) && IsValid(UltimateMontage2))
		{
			GetCharacterMovement()->MaxWalkSpeed = 0.0f;
			MyAnimInstance->Montage_Play(UltimateMontage2, UltimatePlayRate);
			const float RawLength = UltimateMontage2->GetPlayLength();
			const float StartUltimateMontageDuration2 = RawLength / UltimatePlayRate;
			GetWorld()->GetTimerManager().SetTimer(TimerHandle_BeginUltimateBehaviour2, this, &ARP_Character::BeginUltimateBehaviour2, StartUltimateMontageDuration2, false);
		}
		else {
			BeginUltimateBehaviour2();
		}
		BP_StartUltimate2();
	}
}

void ARP_Character::StopUltimate2()
{

}

void ARP_Character::GoToMainMenu()
{
	if (IsValid(GameInstanceReference))
	{
		GameInstanceReference->SaveData();
	}

	UGameplayStatics::OpenLevel(GetWorld(), MainMenuMapName);
}

void ARP_Character::ChangeWaepon()
{
	bUsingPrimaryWeapon = !bUsingPrimaryWeapon;
	OnWeaponChanged.Broadcast(bUsingPrimaryWeapon);

	bIsChangingWeapon = true;

	if (IsValid(MyAnimInstance) && IsValid(ChangeWeaponMontage))
	{
		MyAnimInstance->Montage_Play(ChangeWeaponMontage);
		OnChangeWeaponMontageEnded.BindUObject(this, &ARP_Character::HandleChangeWeaponMontageEnded);
		MyAnimInstance->Montage_SetEndDelegate(OnChangeWeaponMontageEnded, ChangeWeaponMontage);
	}
}

void ARP_Character::HandleChangeWeaponMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (bInterrupted)
	{
		OnWeaponChanged.Broadcast(bUsingPrimaryWeapon);
	}

	if (IsValid(CurrentWeapon))
	{
		CurrentWeapon->Destroy();
		CurrentWeapon = nullptr;
	}

	TSubclassOf<ARP_Weapon> ToSpawn = bUsingPrimaryWeapon ? InitialWeaponClass : SecondaryWeaponClass;

	if (ToSpawn)
	{
		CurrentWeapon = GetWorld()->SpawnActor<ARP_Weapon>(ToSpawn, GetActorLocation(), GetActorRotation());
		if (CurrentWeapon)
		{
			CurrentWeapon->SetCharacterOwner(this);
			CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		}
	}
	bIsChangingWeapon = false;
	OnChangeWeaponMontageEnded.Unbind();
}

void ARP_Character::MakeMeleeDamage(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (IsValid(OtherActor))
	{
		if (OtherActor == this)
		{
			return;
		}
		ARP_Character* MeleeTarget = Cast<ARP_Character>(OtherActor);
		if (IsValid(MeleeTarget))
		{
			bool bPlayerAttackingEnemy = GetCharacterType() == ERP_CharacterType::CharacterType_Player && MeleeTarget->GetCharacterType() == ERP_CharacterType::CharacterType_Enemy;
			bool bEnemyAttackingPlayer = GetCharacterType() == ERP_CharacterType::CharacterType_Enemy && MeleeTarget->GetCharacterType() == ERP_CharacterType::CharacterType_Player;

			if (bPlayerAttackingEnemy || bEnemyAttackingPlayer)
			{
				UGameplayStatics::ApplyPointDamage(OtherActor, MeleeDamage * CurrentComboMultiplier, SweepResult.Location, SweepResult, GetInstigatorController(), this, nullptr);
			}
		}
		else {
			UGameplayStatics::ApplyPointDamage(OtherActor, MeleeDamage * CurrentComboMultiplier, SweepResult.Location, SweepResult, GetInstigatorController(), this, nullptr);
		}
	}
}

void ARP_Character::MakeUltimate2Damage(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (IsValid(OtherActor) && OtherActor != this)
	{
		UGameplayStatics::ApplyPointDamage(OtherActor, UltimateCollisionDamage, SweepResult.Location, SweepResult, GetInstigatorController(), this, nullptr);
	}
}

void ARP_Character::OnHealthChange(URP_HealthComponent* CurrentHealthComponent, AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{

	if (CurrentHealthComponent->IsDead())
	{
		CurrentHealthComponent->OnHealthChangeDelegate.RemoveDynamic(this, &ARP_Character::OnHealthChange);

		if (GetCharacterType() == ERP_CharacterType::CharacterType_Player)
		{
			if (IsValid(GameModeReference))
				GameModeReference->GameOver(this);
		}
		else
		{
			SetActorEnableCollision(false);
			SetLifeSpan(10.0f);
		}
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

	PlayerInputComponent->BindAction("Ultimate", IE_Pressed, this, &ARP_Character::StartUltimate);
	PlayerInputComponent->BindAction("Ultimate", IE_Released, this, &ARP_Character::StopUltimate);

	PlayerInputComponent->BindAction("Ultimate2", IE_Pressed, this, &ARP_Character::StartUltimate2);
	PlayerInputComponent->BindAction("Ultimate2", IE_Released, this, &ARP_Character::StopUltimate2);

	PlayerInputComponent->BindAction("Exit", IE_Pressed, this, &ARP_Character::GoToMainMenu);

	PlayerInputComponent->BindAction("ChangeWeapon", IE_Pressed, this, &ARP_Character::ChangeWaepon);
}

void ARP_Character::AddKey(FName NewKey)
{
	DoorKeys.Add(NewKey);
}

bool ARP_Character::TryAddHealth(float HealthToAdd)
{
	return HealthComponent->TryAddHealth(HealthToAdd);
}

bool ARP_Character::TryDesactivateBotSpawn(bool DesactivateBotSpawn)
{
	return true;
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

void ARP_Character::GainUltimateXP(float XPGained)
{
	if (bCanUseUltimate || bIsUsingUltimate || bIsUsingUltimate2)
	{
		return;
	}

	CurrentUltimateXP = FMath::Clamp(CurrentUltimateXP + XPGained, 0.0f, MaxUltimateXP);
	OnUltimateUpdateDelegate.Broadcast(CurrentUltimateXP, MaxUltimateXP);

	if (CurrentUltimateXP == MaxUltimateXP)
	{
		bCanUseUltimate = true;
		OnUltimateStatusDelegate.Broadcast(true);
	}

	BP_GainUltimateXP(XPGained);
}

void ARP_Character::UpdateUltimateDuration(float Value)
{
	CurrentUltimateDuration = FMath::Clamp(CurrentUltimateDuration - Value, 0.0f, MaxUltimateDuration);
	OnUltimateUpdateDelegate.Broadcast(CurrentUltimateDuration, MaxUltimateDuration);
	BP_UpdateUltimateDuration(Value);

	if (CurrentUltimateDuration == 0.0f)
	{
		bIsUsingUltimate = false;
		OnUltimateStatusDelegate.Broadcast(false);
		PlayRate = 1.0f;

		GetCharacterMovement()->MaxWalkSpeed = NormalWalkSpeed;
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle_AutomaticShoot);

		if (!bUltimateWithTick)
		{
			GetWorld()->GetTimerManager().ClearTimer(TimerHandle_Ultimate);
		}
		CurrentUltimateXP = 0.0f;
		BP_StopUltimate();
	}
}

void ARP_Character::UpdateUltimateDurationWithTimer()
{
	UpdateUltimateDuration(UltimateFrequency);
}

void ARP_Character::BeginUltimateBehaviour()
{
	bIsUsingUltimate = true;

	GetCharacterMovement()->MaxWalkSpeed = UltimateWalkSpeed;
	PlayRate = UltimatePlayRate;

	if (!bUltimateWithTick)
	{
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_Ultimate, this, &ARP_Character::UpdateUltimateDurationWithTimer, UltimateFrequency, true);
	}
}

void ARP_Character::UpdateUltimateDuration2(float Value)
{
	CurrentUltimateDuration2 = FMath::Clamp(CurrentUltimateDuration2 - Value, 0.0f, MaxUltimateDuration2);
	OnUltimateUpdateDelegate.Broadcast(CurrentUltimateDuration2, MaxUltimateDuration2);
	BP_UpdateUltimateDuration2(Value);

	if (CurrentUltimateDuration2 == 0.0f)
	{
		bIsUsingUltimate2 = false;
		OnUltimateStatusDelegate.Broadcast(false);
		PlayRate = 1.0f;
		SetInvulnerable(false);

		GetCharacterMovement()->MaxWalkSpeed = NormalWalkSpeed;

		MeleeDetectorComponent3->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		if (!bUltimateWithTick)
		{
			GetWorld()->GetTimerManager().ClearTimer(TimerHandle_Ultimate2);
		}
		CurrentUltimateXP = 0.0f;
		BP_StopUltimate2();
	}
}

void ARP_Character::UpdateUltimateDurationWithTimer2()
{
	UpdateUltimateDuration2(UltimateFrequency);
}

void ARP_Character::BeginUltimateBehaviour2()
{
	bIsUsingUltimate2 = true;

	GetCharacterMovement()->MaxWalkSpeed = UltimateWalkSpeed;
	PlayRate = UltimatePlayRate;

	MeleeDetectorComponent3->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	if (!bUltimateWithTick)
	{
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_Ultimate2, this, &ARP_Character::UpdateUltimateDurationWithTimer2, UltimateFrequency, true);
	}
}

void ARP_Character::SetInvulnerable(bool bNewInvulnerable)
{
	bIsInvulnerable = bNewInvulnerable;

	if (HealthComponent)
	{
		HealthComponent->InvulnerableState(bIsInvulnerable);
	}

    if (CurrentWeapon)
    {
		CurrentWeapon->InvulnerableState(bIsInvulnerable);
    }
}


