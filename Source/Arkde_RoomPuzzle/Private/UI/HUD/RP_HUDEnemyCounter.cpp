// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUD/RP_HUDEnemyCounter.h"
#include "Core/RP_GameInstance.h"

void URP_HUDEnemyCounter::InitializedWidget()
{
	GameInstanceReference = Cast<URP_GameInstance>(GetWorld()->GetGameInstance());
	if (IsValid(GameInstanceReference))
	{
		GameInstanceReference->OnEnemyDefeatedDelegate.AddDynamic(this, &URP_HUDEnemyCounter::UpdateCounter);
		UpdateCounter(GameInstanceReference->GetEnemierDefeatedCounter());
	}
}

void URP_HUDEnemyCounter::UpdateCounter(int EnemyDefeatedCounter)
{
	EnemiesDefeated = EnemyDefeatedCounter;
}
