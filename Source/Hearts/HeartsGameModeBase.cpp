// Copyright Epic Games, Inc. All Rights Reserved.


#include "HeartsGameModeBase.h"


void AHeartsGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Orange, FString::Printf(TEXT("GAME MODE HAS LOADED FOR FIRST TIME")));
	}
}

