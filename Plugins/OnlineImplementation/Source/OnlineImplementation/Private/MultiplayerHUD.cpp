// Fill out your copyright notice in the Description page of Project Settings.


#include "MultiplayerHUD.h"

#include "MultiplayerWidget.h"
#include "Blueprint/UserWidget.h"

AMultiplayerHUD::AMultiplayerHUD() :
	MultiplayerWidgetClass(UMultiplayerWidget::StaticClass())
{
}

void AMultiplayerHUD::BeginPlay()
{
	Super::BeginPlay();

	if(MultiplayerWidgetClass)
	{
		// CurrentWidget = CreateWidget(GetOwningPlayerController(), MultiplayerWidgetClass);
		// CurrentWidget->AddToViewport();
	}
}
