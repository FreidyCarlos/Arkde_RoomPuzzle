// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RP_HUDWeaponIcon.generated.h"

class UImage;
class UTexture2D;
class ARP_Character;

/**
 * 
 */
UCLASS()
class ARKDE_ROOMPUZZLE_API URP_HUDWeaponIcon : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta = (BindWidget))
	UImage* WeaponIcon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WeaponIcon")
	UTexture2D* PrimaryWeaponTexture;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WeaponIcon")
	UTexture2D* SecondaryWeaponTexture;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Setup")
    FName RifleName;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Setup")
    FName GrenadeLauncherName;

    UPROPERTY(BlueprintReadOnly, Category = "Setup")
    FName CurrentWeaponText;

	UFUNCTION()
	void UpdateWeaponIcon(bool bUsingPrimary);

private:
	UPROPERTY()
	ARP_Character* CachedCharacter;

	UFUNCTION()
	void HandleWeaponChanged(bool bNowUsingPrimary);

public:

	UFUNCTION(BlueprintCallable, Category = "WeaponIcon")
	void InitializeWidget();
};
