// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "APILearningGameMode.generated.h"

UCLASS(minimalapi)
class AAPILearningGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AAPILearningGameMode();

	virtual void PostLogin(APlayerController* NewPlayer) override;
};



