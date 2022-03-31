// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "HeartsGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class HEARTS_API AHeartsGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
		UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FText> GameModeBaseSavedGameSlotNameArray;

	
protected:

	virtual void BeginPlay() override;
	
};
