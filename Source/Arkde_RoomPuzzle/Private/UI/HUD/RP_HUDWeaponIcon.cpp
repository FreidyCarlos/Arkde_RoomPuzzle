// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUD/RP_HUDWeaponIcon.h"
#include "Components/Image.h"
#include "Engine/Texture2D.h"
#include "RP_Character.h"
#include "Blueprint/WidgetTree.h"
#include "Kismet/GameplayStatics.h"

void URP_HUDWeaponIcon::InitializeWidget()
{
    if (APlayerController* PC = GetOwningPlayer())
    {
        CachedCharacter = Cast<ARP_Character>(PC->GetPawn());
        if (CachedCharacter)
        {
            CachedCharacter->OnWeaponChanged.AddDynamic(this, &URP_HUDWeaponIcon::HandleWeaponChanged);
            UpdateWeaponIcon(CachedCharacter->IsUsingPrimaryWeapon());
        }
    }
}

void URP_HUDWeaponIcon::UpdateWeaponIcon(bool bUsingPrimary)
{
	if (!WeaponIcon)
	{
		return;
	}

    if (bUsingPrimary)
    {
        CurrentWeaponText = RifleName;
    }
    else
    {
        CurrentWeaponText = GrenadeLauncherName;
    }

    UTexture2D* NewTexture = bUsingPrimary ? PrimaryWeaponTexture : SecondaryWeaponTexture;
    if (NewTexture)
    {
        WeaponIcon->SetBrushFromTexture(NewTexture, true);
    }
}

void URP_HUDWeaponIcon::HandleWeaponChanged(bool bNowUsingPrimary)
{
    UpdateWeaponIcon(bNowUsingPrimary);
}

