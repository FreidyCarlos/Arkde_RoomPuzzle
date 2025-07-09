// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RP_PauseMenuWidget.generated.h"

/**
 * 
 */
UCLASS()
class ARKDE_ROOMPUZZLE_API URP_PauseMenuWidget : public UUserWidget
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pause Menu")
	FName GameplayLevelGame;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pause Menu")
	FName MainMenuLevel;
	
protected:

	UFUNCTION(BlueprintCallable, Category = "Pause Menu")
	void ReturnToMainMenu();

	UFUNCTION(BlueprintCallable, Category = "Pause Menu")
	void ReloadGame();

	UFUNCTION(BlueprintCallable, Category = "Pause Menu")
	void SaveGame();
};
