// Fill out your copyright notice in the Description page of Project Settings.


#include "MultiplayerWidget.h"

#include "MultiplayerSubsystem.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/TextBlock.h"

bool UMultiplayerWidget::Initialize()
{
	return Super::Initialize();
}

void UMultiplayerWidget::CreateSession()
{
	if(const ULocalPlayer* LocalPlayer = GetOwningPlayer()->GetLocalPlayer())
	{
		if(UMultiplayerSubsystem* MultiplayerSubsystem = LocalPlayer->GetSubsystem<UMultiplayerSubsystem>())
		{
			MultiplayerSubsystem->CreateSession(4, FName(TEXT("Session")));
		}
	}
}

void UMultiplayerWidget::FindSession()
{
	if(const ULocalPlayer* LocalPlayer = GetOwningPlayer()->GetLocalPlayer())
	{
		if(UMultiplayerSubsystem* MultiplayerSubsystem = LocalPlayer->GetSubsystem<UMultiplayerSubsystem>())
		{
			MultiplayerSubsystem->FindSession(1000);
		}
	}
}

void UMultiplayerWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	/*if(Button_Host)*/ Button_Host->OnClicked.AddDynamic(this, &ThisClass::CreateSession);			
	/*if(Button_Join)*/ Button_Join->OnClicked.AddDynamic(this, &ThisClass::FindSession);			
}

void UMultiplayerWidget::NativeDestruct()
{
	Super::NativeDestruct();
}

TSharedRef<SWidget> UMultiplayerWidget::RebuildWidget()
{
	Panel = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("Canvas Panel"));
	WidgetTree->RootWidget = Panel;

	Button_Host = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), TEXT("Button_Host"));
	Button_Join = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), TEXT("Button_Join"));

	Panel->AddChildToCanvas(Button_Host);
	Panel->AddChildToCanvas(Button_Join);

	FWidgetTransform Transform;
	
	Transform.Translation = {100.f, 100.f};
	Button_Host->SetRenderTransform(Transform);

	Transform.Translation = {100.f, 300.f};
	Button_Join->SetRenderTransform(Transform);

	static FName sTextHost(TEXT("Join"));
	static FName sTextJoin(TEXT("Host"));

	Text_Host = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("Text_Host"));
	Text_Host->SetText(FText::FromName(sTextHost));
	
	Text_Join = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("Text_Join"));
	Text_Join->SetText(FText::FromName(sTextJoin));
	
	return Super::RebuildWidget();
}
