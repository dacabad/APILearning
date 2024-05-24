// Copyright Epic Games, Inc. All Rights Reserved.

#include "APILearningGameMode.h"
#include "APILearningCharacter.h"
#include "MultiplayerHUD.h"
#include "UObject/ConstructorHelpers.h"

AAPILearningGameMode::AAPILearningGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/Dummy"));
	if (PlayerPawnBPClass.Class != nullptr)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	HUDClass = AMultiplayerHUD::StaticClass();
}

void AAPILearningGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	UE_LOG(LogTemp, Display, TEXT("%s logged in"), *NewPlayer->GetName())
}
