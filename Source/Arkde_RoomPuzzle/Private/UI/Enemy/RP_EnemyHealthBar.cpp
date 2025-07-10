// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Enemy/RP_EnemyHealthBar.h"

void URP_EnemyHealthBar::UpdateHealth(float CurrentHealth, float MaxHealth)
{
	HealthPercent = CurrentHealth / MaxHealth;
}
