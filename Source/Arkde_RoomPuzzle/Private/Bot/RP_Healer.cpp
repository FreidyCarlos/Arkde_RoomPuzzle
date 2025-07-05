// RP_Healer.cpp
// Fill out your copyright notice in the Description page of Project Settings.

#include "Bot/RP_Healer.h"
#include "Components/StaticMeshComponent.h"
#include "Components/RP_HealthComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/SphereComponent.h"
#include "DrawDebugHelpers.h"
#include "RP_Character.h"
#include "Weapons/RP_Rifle.h"
#include "Weapons/RP_Projectile.h"
#include "Weapons/RP_GrenadeLauncher.h"
#include "Items/RP_Item.h"
#include "Enemy/RP_HealerSpawner.h"
#include "Items/RP_HealerSpawnDesactivator.h"
#include "Core/RP_GameInstance.h"

// Sets default values
ARP_Healer::ARP_Healer()
{
    // Set this pawn to call Tick() every frame.
    PrimaryActorTick.bCanEverTick = true;

    BotMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BotMeshComponent"));
    BotMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    BotMeshComponent->SetCanEverAffectNavigation(false);
    BotMeshComponent->SetSimulatePhysics(false);
    RootComponent = BotMeshComponent;

    HealthComponent = CreateDefaultSubobject<URP_HealthComponent>(TEXT("HealthComponent"));

    HealingSphere = CreateDefaultSubobject<USphereComponent>(TEXT("HealingSphere"));
    HealingSphere->SetupAttachment(RootComponent);
    HealingSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    HealingSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
    HealingSphere->SetCollisionResponseToChannel(ECC_Pawn, ECollisionResponse::ECR_Overlap);
    HealRadius = 600.0f;
    HealingSphere->SetSphereRadius(HealRadius);

    PatrolAngle = 0.0f;
    PatrolRadius = 250.0f;
    PatrolSpeed = 1.0f;
    bIsPatrolling = true;

    HealAmountPerTick = 10.f;
    HealInterval = 1.0f;
    HealerState = EHealerState::Patrolling;
    CurrentTargetHealthComp = nullptr;
    ActiveHealingEmitterComponent = nullptr;

    XPValue = 20.0;

    LootProbability = 70.0f;
    DesactivatorProbability = 25.0f;
}

// Called when the game starts or when spawned
void ARP_Healer::BeginPlay()
{
    Super::BeginPlay();

    PatrolCenter = BotMeshComponent->GetComponentLocation();

    if (BotMeshComponent->IsSimulatingPhysics())
    {
        BotMeshComponent->SetEnableGravity(false);
    }

    GameInstanceReference = Cast<URP_GameInstance>(GetWorld()->GetGameInstance());

    HealthComponent->OnHealthChangeDelegate.AddDynamic(this, &ARP_Healer::TakingDamage);
    HealthComponent->OnDeadDelegate.AddDynamic(this, &ARP_Healer::GiveXP);

    HealerMaterial = BotMeshComponent->CreateAndSetMaterialInstanceDynamicFromMaterial(0, BotMeshComponent->GetMaterial(0));

    HealingSphere->OnComponentBeginOverlap.AddDynamic(this, &ARP_Healer::OnHealingSphereBeginOverlap);
    HealingSphere->OnComponentEndOverlap.AddDynamic(this, &ARP_Healer::OnHealingSphereEndOverlap);

    ProcessCurrentOverlaps();
}

void ARP_Healer::ProcessCurrentOverlaps()
{
    if (!HealingSphere) return;
    TArray<AActor*> Overlapping;
    HealingSphere->GetOverlappingActors(Overlapping, ARP_Character::StaticClass());
    for (AActor* Act : Overlapping)
    {
        if (!Act || Act == this) continue;
        ARP_Character* Char = Cast<ARP_Character>(Act);
        if (!Char || Char->GetCharacterType() != ERP_CharacterType::CharacterType_Enemy) continue;
        URP_HealthComponent* HC = Act->FindComponentByClass<URP_HealthComponent>();
        if (!HC || !IsValid(HC)) continue;

        HC->OnHealthChangeDelegate.RemoveDynamic(this, &ARP_Healer::OnTargetHealthChanged);
        HC->OnHealthChangeDelegate.AddDynamic(this, &ARP_Healer::OnTargetHealthChanged);

        if (!HC->IsDead() && HC->GetHealth() < HC->GetMaxHealth())
        {
            DamagedTargets.AddUnique(HC);
            if (HealerState == EHealerState::Patrolling || HealerState == EHealerState::ReturningToPatrol)
            {
                CurrentTargetHealthComp = FindNearestDamaged();
                if (CurrentTargetHealthComp)
                {
                    BeginMoveToTarget();
                }
            }
        }
    }
}

bool ARP_Healer::IsInHealingSphere(URP_HealthComponent* HealthComp) const
{
    if (!HealthComp) return false;
    AActor* ActorOwner = HealthComp->GetOwner();
    return ActorOwner && HealingSphere && HealingSphere->IsOverlappingActor(ActorOwner);
}

void ARP_Healer::TakingDamage(URP_HealthComponent* CurrentHealthComponent, AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
    if (IsValid(HealerMaterial))
    {
        HealerMaterial->SetScalarParameterValue("Pulse", GetWorld()->TimeSeconds);
    }
    if (HealthComponent->IsDead())
    {
        if (IsValid(DamageCauser))
        {
            ARP_Rifle* Rifle = Cast<ARP_Rifle>(DamageCauser);
            if (IsValid(Rifle))
            {
                ARP_Character* RifleOwner = Cast<ARP_Character>(Rifle->GetOwner());
                if (IsValid(RifleOwner) && RifleOwner->GetCharacterType() == ERP_CharacterType::CharacterType_Player)
                {
                    TrySpawnLoot();
                }
            }
            else
            {
                ARP_Projectile* Projectile = Cast<ARP_Projectile>(DamageCauser);
                if (IsValid(Projectile))
                {
                    ARP_GrenadeLauncher* ProjectileCaster = Cast<ARP_GrenadeLauncher>(Projectile->GetOwner());

                    if (IsValid(ProjectileCaster))
                    {
                        ARP_Character* GrenadeLauncherOwner = Cast<ARP_Character>(ProjectileCaster->GetOwner());

                        if (IsValid(GrenadeLauncherOwner) && GrenadeLauncherOwner->GetCharacterType() == ERP_CharacterType::CharacterType_Player)
                        {
                            GrenadeLauncherOwner->GainUltimateXP(XPValue);

                            TrySpawnLoot();
                        }
                    }
                }
            }
        }

        if (IsValid(GameInstanceReference))
        {
            GameInstanceReference->AddEnemyDefeatedToCounter();
        }

        Destruction();
    }
}

void ARP_Healer::Destruction()
{
    if (bIsDead)
    {
        return;
    }

    bIsDead = true;

    if (IsValid(DeadEffect))
    {
        UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), DeadEffect, GetActorLocation());
    }

    if (IsValid(MySpawner))
    {
        MySpawner->NotifyBotDead();
    }

    Destroy();
}

void ARP_Healer::OnHealingSphereBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (!HasAuthority() || !OtherActor || OtherActor == this) return;

    ARP_Character* OtherChar = Cast<ARP_Character>(OtherActor);
    if (!OtherChar || OtherChar->GetCharacterType() != ERP_CharacterType::CharacterType_Enemy) return;

    URP_HealthComponent* HC = OtherActor->FindComponentByClass<URP_HealthComponent>();
    if (!HC || !IsValid(HC)) return;

    // Suscribir para cambios de salud (incluso si está a full health, para detectar futuro daño)
    HC->OnHealthChangeDelegate.RemoveDynamic(this, &ARP_Healer::OnTargetHealthChanged);
    HC->OnHealthChangeDelegate.AddDynamic(this, &ARP_Healer::OnTargetHealthChanged);

    // Si está herido, agregar a la lista y posiblemente iniciar curación
    if (!HC->IsDead() && HC->GetHealth() < HC->GetMaxHealth())
    {
        DamagedTargets.AddUnique(HC);
        TrySelectNewTarget();
    }
}

void ARP_Healer::OnHealingSphereEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    if (!HasAuthority() || !OtherActor || OtherActor == this) return;

    ARP_Character* OtherChar = Cast<ARP_Character>(OtherActor);
    if (!OtherChar || OtherChar->GetCharacterType() != ERP_CharacterType::CharacterType_Enemy) return;

    URP_HealthComponent* HC = OtherActor->FindComponentByClass<URP_HealthComponent>();
    if (!HC) return;

    // Quitar de la lista y desuscribir
    DamagedTargets.RemoveAll([HC](URP_HealthComponent* Elem) { return Elem == HC; });
    HC->OnHealthChangeDelegate.RemoveDynamic(this, &ARP_Healer::OnTargetHealthChanged);

    if (CurrentTargetHealthComp == HC)
    {
        StopHealing();
        CurrentTargetHealthComp = nullptr;
        SelectNextOrReturn();
    }
}

void ARP_Healer::OnTargetHealthChanged(URP_HealthComponent* HealthComp, AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
    if (!HasAuthority() || !HealthComp || !IsValid(HealthComp)) return;

    // Si murió o llegó a salud máxima
    if (HealthComp->IsDead() || HealthComp->GetHealth() >= HealthComp->GetMaxHealth())
    {
        // Eliminar de lista de curación activa
        DamagedTargets.RemoveAll([HealthComp](URP_HealthComponent* Elem) { return Elem == HealthComp; });

        // Si ya no está en la esfera, desuscribir; si sigue en esfera, mantenemos suscripción
        if (!IsInHealingSphere(HealthComp))
        {
            HealthComp->OnHealthChangeDelegate.RemoveDynamic(this, &ARP_Healer::OnTargetHealthChanged);
        }
        // Si era el target actual, detener curación y retornar a patrulla
        if (CurrentTargetHealthComp == HealthComp)
        {
            StopHealing();
            CurrentTargetHealthComp = nullptr;
            HealerState = EHealerState::ReturningToPatrol;
        }
        return;
    }

    // Sigue vivo y herido: suscribir y agregar
    HealthComp->OnHealthChangeDelegate.RemoveDynamic(this, &ARP_Healer::OnTargetHealthChanged);
    HealthComp->OnHealthChangeDelegate.AddDynamic(this, &ARP_Healer::OnTargetHealthChanged);
    DamagedTargets.AddUnique(HealthComp);

    // Interrumpir estado actual y mover al nuevo target sin modificar LastPatrolLocation
    CurrentTargetHealthComp = FindNearestDamaged();
    if (CurrentTargetHealthComp)
    {
        BeginMoveToTarget();
    }
}

void ARP_Healer::BeginMoveToTarget()
{
    if (!CurrentTargetHealthComp) return;

    // Solo guardar si venimos de Patrolling
    if (HealerState == EHealerState::Patrolling)
    {
        LastPatrolLocation = GetActorLocation();
    }
    HealerState = EHealerState::MovingToTarget;
    bIsPatrolling = false;
}

void ARP_Healer::SelectNextOrReturn()
{
    URP_HealthComponent* Next = FindNearestDamaged();
    if (Next)
    {
        CurrentTargetHealthComp = Next;
        BeginMoveToTarget();
    }
    else
    {
        HealerState = EHealerState::ReturningToPatrol;
    }
}

void ARP_Healer::UpdatePatrol(float DeltaTime)
{
    PatrolAngle += DeltaTime * PatrolSpeed;
    float X = FMath::Cos(PatrolAngle) * PatrolRadius;
    float Y = FMath::Sin(PatrolAngle) * PatrolRadius;
    float Z = PatrolCenter.Z + FMath::Sin(PatrolAngle);
    SetActorLocation(PatrolCenter + FVector(X, Y, Z), false);
}

void ARP_Healer::TrySelectNewTarget()
{
    URP_HealthComponent* Nearest = FindNearestDamaged();
    if (!Nearest) return;

    if (!CurrentTargetHealthComp)
    {
        CurrentTargetHealthComp = Nearest;
        BeginMoveToTarget();
    }
    else
    {
        float CurrentPct = CurrentTargetHealthComp->GetHealth() / CurrentTargetHealthComp->GetMaxHealth();
        float NewPct = Nearest->GetHealth() / Nearest->GetMaxHealth();
        if (NewPct < CurrentPct)
        {
            StopHealing();
            CurrentTargetHealthComp = Nearest;
            BeginMoveToTarget();
        }
    }
}

void ARP_Healer::StartHealing()
{
    if (!HasAuthority()) return;
    if (!CurrentTargetHealthComp || !IsValid(CurrentTargetHealthComp)) return;
    if (CurrentTargetHealthComp->IsDead()) return;

    AActor* TargetActor = CurrentTargetHealthComp->GetOwner();
    if (!TargetActor || !IsValid(TargetActor)) return;

    if (!GetWorldTimerManager().IsTimerActive(HealTimerHandle))
    {
        HealTick();
        GetWorldTimerManager().SetTimer(HealTimerHandle, this, &ARP_Healer::HealTick, HealInterval, true);
    }

    if (HealingEffect && !ActiveHealingEmitterComponent)
    {
        USkeletalMeshComponent* MeshComp = TargetActor->FindComponentByClass<USkeletalMeshComponent>();
        if (MeshComp && IsValid(MeshComp))
        {
            const FName SocketName(TEXT("SCK_Healing"));
            ActiveHealingEmitterComponent = UGameplayStatics::SpawnEmitterAttached(
                HealingEffect,
                MeshComp,
                SocketName,
                FVector::ZeroVector,
                FRotator::ZeroRotator,
                EAttachLocation::SnapToTarget,
                true
            );
        }
    }
}

void ARP_Healer::StopHealing()
{
    if (GetWorldTimerManager().IsTimerActive(HealTimerHandle))
    {
        GetWorldTimerManager().ClearTimer(HealTimerHandle);
    }
    if (ActiveHealingEmitterComponent)
    {
        ActiveHealingEmitterComponent->DestroyComponent();
        ActiveHealingEmitterComponent = nullptr;
    }
}

void ARP_Healer::HealTick()
{
    if (!HasAuthority()) return;
    if (!CurrentTargetHealthComp || !IsValid(CurrentTargetHealthComp) || CurrentTargetHealthComp->IsDead())
    {
        StopHealing();
        return;
    }
    float Old = CurrentTargetHealthComp->GetHealth();
    float MaxH = CurrentTargetHealthComp->GetMaxHealth();
    if (Old >= MaxH)
    {
        StopHealing();
        CurrentTargetHealthComp = nullptr;
        HealerState = EHealerState::ReturningToPatrol;
        return;
    }

    float NewH = FMath::Min(Old + HealAmountPerTick, MaxH);
    CurrentTargetHealthComp->SetHealth(NewH);
    UE_LOG(LogTemp, Log, TEXT("Healer: %s salud %.1f -> %.1f"),
        *CurrentTargetHealthComp->GetOwner()->GetName(), Old, NewH);

    if (NewH >= MaxH)
    {
        // Solo quitar de la lista de DamagedTargets
        DamagedTargets.RemoveAll([this](URP_HealthComponent* Elem) { return Elem == CurrentTargetHealthComp; });
        StopHealing();

        // Verificar si el actor sigue dentro de la esfera:
        if (!IsInHealingSphere(CurrentTargetHealthComp))
        {
            // Si salió, quitar suscripción para ahorrar callbacks futuros
            CurrentTargetHealthComp->OnHealthChangeDelegate.RemoveDynamic(this, &ARP_Healer::OnTargetHealthChanged);
        }
        // Si sigue dentro, mantenemos la suscripción para captar futuros daños.

        CurrentTargetHealthComp = nullptr;
        HealerState = EHealerState::ReturningToPatrol;
    }
}

URP_HealthComponent* ARP_Healer::FindNearestDamaged()
{
    URP_HealthComponent* Best = nullptr;
    float BestDist2 = TNumericLimits<float>::Max();

    for (URP_HealthComponent* HC : DamagedTargets)
    {
        if (!HC || !IsValid(HC)) continue;
        if (HC->IsDead() || HC->GetHealth() >= HC->GetMaxHealth()) continue;
        AActor* TargetActor = HC->GetOwner();
        if (!TargetActor || !IsValid(TargetActor)) continue;

        float Dist2 = FVector::DistSquared(GetActorLocation(), TargetActor->GetActorLocation());
        if (Dist2 < BestDist2)
        {
            BestDist2 = Dist2;
            Best = HC;
        }
    }
    return Best;
}

void ARP_Healer::MoveTowards(const FVector& TargetLocation, float DeltaTime)
{
    FVector Dir = (TargetLocation - GetActorLocation()).GetSafeNormal();
    FVector NewLocation = GetActorLocation() + Dir * TargetMoveSpeed * DeltaTime;
    SetActorLocation(NewLocation, true);
}

void ARP_Healer::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    if (bIsDead) return;

    // Detectar enemigos overlapeados durante movimiento o retorno
    if (HealerState == EHealerState::MovingToTarget || HealerState == EHealerState::ReturningToPatrol)
    {
        TArray<AActor*> Overlapping;
        HealingSphere->GetOverlappingActors(Overlapping, ARP_Character::StaticClass());
        for (AActor* Act : Overlapping)
        {
            if (!Act || Act == this) continue;
            ARP_Character* Char = Cast<ARP_Character>(Act);
            if (!Char || Char->GetCharacterType() != ERP_CharacterType::CharacterType_Enemy) continue;
            URP_HealthComponent* HC = Act->FindComponentByClass<URP_HealthComponent>();
            if (!HC || !IsValid(HC)) continue;
            if (HC->IsDead() || HC->GetHealth() >= HC->GetMaxHealth()) continue;

            HC->OnHealthChangeDelegate.RemoveDynamic(this, &ARP_Healer::OnTargetHealthChanged);
            HC->OnHealthChangeDelegate.AddDynamic(this, &ARP_Healer::OnTargetHealthChanged);
            DamagedTargets.AddUnique(HC);

            CurrentTargetHealthComp = FindNearestDamaged();
            if (CurrentTargetHealthComp)
            {
                BeginMoveToTarget();
            }
            break;
        }
    }

    // Limpiar DamagedTargets inválidos
    DamagedTargets.RemoveAll([this](URP_HealthComponent* Elem) {
        return !Elem || !IsValid(Elem) || Elem->IsDead() || Elem->GetHealth() >= Elem->GetMaxHealth();
        });

    switch (HealerState)
    {
    case EHealerState::Patrolling:
        UpdatePatrol(DeltaTime);
        break;

    case EHealerState::MovingToTarget:
        if (!CurrentTargetHealthComp || !IsValid(CurrentTargetHealthComp) ||
            CurrentTargetHealthComp->IsDead() ||
            CurrentTargetHealthComp->GetHealth() >= CurrentTargetHealthComp->GetMaxHealth())
        {
            CurrentTargetHealthComp = nullptr;
            SelectNextOrReturn();
            return;
        }
        {
            AActor* TargetActor = CurrentTargetHealthComp->GetOwner();
            if (!TargetActor || !IsValid(TargetActor))
            {
                CurrentTargetHealthComp = nullptr;
                SelectNextOrReturn();
                return;
            }
            float Dist = FVector::Dist(GetActorLocation(), TargetActor->GetActorLocation());
            if (Dist <= MinHealDistance)
            {
                HealerState = EHealerState::Healing;
                StartHealing();
            }
            else
            {
                MoveTowards(TargetActor->GetActorLocation(), DeltaTime);
            }
        }
        break;

    case EHealerState::Healing:
        if (!CurrentTargetHealthComp || !IsValid(CurrentTargetHealthComp) ||
            CurrentTargetHealthComp->IsDead() ||
            CurrentTargetHealthComp->GetHealth() >= CurrentTargetHealthComp->GetMaxHealth())
        {
            StopHealing();
            CurrentTargetHealthComp = nullptr;
            HealerState = EHealerState::ReturningToPatrol;
            return;
        }
        {
            AActor* TargetActor = CurrentTargetHealthComp->GetOwner();
            if (!TargetActor || !IsValid(TargetActor))
            {
                StopHealing();
                CurrentTargetHealthComp = nullptr;
                HealerState = EHealerState::ReturningToPatrol;
                return;
            }
            float Dist = FVector::Dist(GetActorLocation(), TargetActor->GetActorLocation());
            if (Dist > MinHealDistance)
            {
                StopHealing();
                HealerState = EHealerState::MovingToTarget;
                return;
            }
            // Curación continua en HealTick()
        }
        break;

    case EHealerState::ReturningToPatrol:
    {
        float Dist = FVector::Dist(GetActorLocation(), LastPatrolLocation);
        const float Threshold = 10.f;
        if (Dist > Threshold)
        {
            MoveTowards(LastPatrolLocation, DeltaTime);
        }
        else
        {
            bIsPatrolling = true;
            FVector Dir = LastPatrolLocation - PatrolCenter;
            if (!Dir.IsNearlyZero())
            {
                PatrolAngle = FMath::Atan2(Dir.Y, Dir.X);
            }
            HealerState = EHealerState::Patrolling;
        }
    }
    break;
    }

    if (bDebug && HealingSphere)
    {
        FVector Center = HealingSphere->GetComponentLocation();
        float Radius = HealingSphere->GetScaledSphereRadius();
        DrawDebugSphere(GetWorld(), Center, Radius, 16, FColor::Green, false, 0.f, 0, 2.0f);
    }
}

void ARP_Healer::GiveXP(AActor* DamageCauser)
{
    ARP_Character* PossiblePlayer = Cast<ARP_Character>(DamageCauser);
    if (IsValid(PossiblePlayer) && PossiblePlayer->GetCharacterType() == ERP_CharacterType::CharacterType_Player)
    {
        PossiblePlayer->GainUltimateXP(XPValue);
    }

    ARP_Rifle* PossibleRifle = Cast<ARP_Rifle>(DamageCauser);
    if (IsValid(PossibleRifle))
    {
        ARP_Character* RifleOwner = Cast<ARP_Character>(PossibleRifle->GetOwner());

        if (IsValid(RifleOwner) && RifleOwner->GetCharacterType() == ERP_CharacterType::CharacterType_Player)
        {
            RifleOwner->GainUltimateXP(XPValue);
        }
    }

    ARP_Projectile* PossibleProjectile = Cast<ARP_Projectile>(DamageCauser);
    if (IsValid(PossibleProjectile))
    {
        ARP_GrenadeLauncher* ProjectileCaster = Cast<ARP_GrenadeLauncher>(PossibleProjectile->GetOwner());


        if (IsValid(ProjectileCaster))
        {
            ARP_Character* GrenadeLauncherOwner = Cast<ARP_Character>(ProjectileCaster->GetOwner());

            if (IsValid(GrenadeLauncherOwner) && GrenadeLauncherOwner->GetCharacterType() == ERP_CharacterType::CharacterType_Player)
            {
                GrenadeLauncherOwner->GainUltimateXP(XPValue);
            }
        }
    }

    BP_GiveXP(DamageCauser);
}

bool ARP_Healer::TrySpawnLoot()
{
    if (!IsValid(LootItemClass) || !IsValid(DesactivatorClass))
    {
        return false;
    }

    float SelectorProobability = FMath::RandRange(0.0f, 100.0f);
    if (SelectorProobability <= LootProbability)
    {
        FActorSpawnParameters SpawnParameter;
        SpawnParameter.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

        GetWorld()->SpawnActor<ARP_Item>(LootItemClass, GetActorLocation(), FRotator::ZeroRotator, SpawnParameter);
    }

    if (SelectorProobability <= DesactivatorProbability && MySpawner->IsSpawnActive())
    {
        FActorSpawnParameters SpawnParameter;
        SpawnParameter.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

        ARP_HealerSpawnDesactivator* Key = GetWorld()->SpawnActor<ARP_HealerSpawnDesactivator>(DesactivatorClass, GetActorLocation(), FRotator::ZeroRotator, SpawnParameter);
        if (IsValid(Key))
        {
            Key->SetOwningSpawner(MySpawner);
            Key->SetIsPick(true);
        }
    }

    return false;
}
