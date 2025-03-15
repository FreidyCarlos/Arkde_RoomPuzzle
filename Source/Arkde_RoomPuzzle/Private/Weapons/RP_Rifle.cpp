// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/RP_Rifle.h"

void ARP_Rifle::StartAction()
{
	Super::StartAction();

	UE_LOG(LogTemp, Log, TEXT("Player starts action"));
}

void ARP_Rifle::StopAction()
{
	Super::StopAction();

	UE_LOG(LogTemp, Log, TEXT("Player stops actions"));
}
