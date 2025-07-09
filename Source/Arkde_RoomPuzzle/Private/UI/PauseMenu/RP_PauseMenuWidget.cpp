// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/PauseMenu/RP_PauseMenuWidget.h"
#include "GameFramework/PlayerController.h"
#include "Core/RP_GameInstance.h"
#include "Kismet/GameplayStatics.h"

void URP_PauseMenuWidget::ReturnToMainMenu()
{
	URP_GameInstance* GameInstanceReference = Cast<URP_GameInstance>(GetWorld()->GetGameInstance());
	if (IsValid(GameInstanceReference))
	{
		GameInstanceReference->SaveData();
	}
	UGameplayStatics::OpenLevel(GetWorld(), MainMenuLevel);
}

void URP_PauseMenuWidget::ReloadGame()
{
	URP_GameInstance* GameInstanceReference = Cast<URP_GameInstance>(GetWorld()->GetGameInstance());
	if (IsValid(GameInstanceReference))
	{
		GameInstanceReference->ResetData();
	}

	UGameplayStatics::OpenLevel(GetWorld(), GameplayLevelGame);
}

void URP_PauseMenuWidget::SaveGame()
{
	URP_GameInstance* GameInstanceReference = Cast<URP_GameInstance>(GetWorld()->GetGameInstance());
	if (IsValid(GameInstanceReference))
	{
		GameInstanceReference->SaveData();
		UE_LOG(LogTemp, Warning, TEXT("Guardado"));
	}
}
