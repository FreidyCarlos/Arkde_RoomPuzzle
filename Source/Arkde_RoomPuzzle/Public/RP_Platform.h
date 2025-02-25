// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RP_Platform.generated.h"

class UStaticMeshComponent;

UCLASS()
class ARKDE_ROOMPUZZLE_API ARP_Platform : public AActor
{
	GENERATED_BODY()

//SECCI�N DE COMPONENTES
protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* PlatformMeshComponent;

	UPROPERTY(VisibleAnywhere, category = "Components")
	USceneComponent* CustomRootComponent;
	
//SECCI�N DE VARIABLES
protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Platform")
	bool bIsGoingUp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Platform")
	float Speed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Platform")
	float MinHeight;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Platform")
	float MaxHeight;

//SECCI�N DE CONSTRUCTOR
public:	
	// Sets default values for this actor's properties
	ARP_Platform();

//SECCI�N DE FUNCIONES
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void Move();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
