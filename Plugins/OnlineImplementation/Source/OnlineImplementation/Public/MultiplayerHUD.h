// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "MultiplayerHUD.generated.h"

/**
 * 
 */
UCLASS()
class ONLINEIMPLEMENTATION_API AMultiplayerHUD : public AHUD
{
	GENERATED_BODY()

	UPROPERTY(Transient)
	UUserWidget* CurrentWidget;

	UPROPERTY(EditAnywhere, Category = "Widget", meta = (AllowPrivateAccess = true))
	TSubclassOf<UUserWidget> MultiplayerWidgetClass;

public:

	AMultiplayerHUD();
	
protected:

	
	
	virtual void BeginPlay() override;
};
