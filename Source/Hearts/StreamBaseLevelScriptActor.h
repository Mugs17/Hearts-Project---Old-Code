// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/LevelScriptActor.h"
#include "AI_Character.h"
#include "HeartsGameModeBase.h"
#include "PlayerCharacter.h"
#include "StreamBaseLevelScriptActor.generated.h"

/**
 * 
 */
UCLASS()
class HEARTS_API AStreamBaseLevelScriptActor : public ALevelScriptActor
{
	GENERATED_BODY()
	

		UFUNCTION(BlueprintCallable, Category = LevelBlueprint)
		void DoSomething()
	{
		GLog->Log("Does something");
	}
protected:

	virtual void BeginPlay() override;

	void LoadLevelVariables();
	AHeartsGameModeBase* HeartsGameMode;

	void SetLevelAIVariables();

public:

	TArray<AActor*> LevelAI;

	APlayerCharacter* PlayerCharacterForLevelStreaming;






};
