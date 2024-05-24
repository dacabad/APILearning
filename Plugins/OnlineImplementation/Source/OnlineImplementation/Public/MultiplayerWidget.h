// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MultiplayerWidget.generated.h"

class UButton;
class UCanvasPanel;
class UTextBlock;

/**
 * 
 */
UCLASS()
class ONLINEIMPLEMENTATION_API UMultiplayerWidget : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Widget", meta = (AllowPrivateAccess = true))	UCanvasPanel* Panel;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Widget", meta = (AllowPrivateAccess = true))	UButton* Button_Host;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Widget", meta = (AllowPrivateAccess = true))	UButton* Button_Join;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Widget", meta = (AllowPrivateAccess = true))	UTextBlock* Text_Host;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Widget", meta = (AllowPrivateAccess = true))	UTextBlock* Text_Join;

protected:

	virtual bool Initialize() override;
	
	UFUNCTION()	void CreateSession();
	UFUNCTION()	void FindSession();

	
	virtual void NativeConstruct() override;

	virtual void NativeDestruct() override;

	virtual TSharedRef<SWidget> RebuildWidget() override;
};
