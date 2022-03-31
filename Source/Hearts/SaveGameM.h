// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "SaveGameM.generated.h"

/**
 * 
 */

//Struct to hold all AI values for the AI that are active, which are only the ones in range
USTRUCT()
struct FInDistanceAICharacterVariableHolder
{
	GENERATED_BODY()

	UPROPERTY()
	FString AIName;
	UPROPERTY()
	float AIUniqueID;
	UPROPERTY()
	float AIHealth;
	UPROPERTY()
	FVector AILocation;
	UPROPERTY()
	FRotator AIControllerRotation;

	UPROPERTY()
	bool bIsAIDead = false;
};

UCLASS()
class HEARTS_API USaveGameM : public USaveGame
{
	GENERATED_BODY()

public:

	USaveGameM();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FText> SaveFileSavedGameSlotNameArray;

	UPROPERTY(EditAnywhere)
		FText SelectedLoadGameSlotName;

	//for player
	UPROPERTY(EditAnywhere)
		FVector PlayerLocation;

	UPROPERTY()
	FRotator PlayerControllerRotation;
	UPROPERTY()
	FRotator PlayerRotation;

	UPROPERTY()
	float PlayerHealth;

	UPROPERTY()
	float bIsPlayerFighting;

	UPROPERTY()
	float PlayerSelectedEnemy;

	UPROPERTY()
	float AtSaveWorldTimeInSeconds;

	//For AI
	UPROPERTY()
	TMap<FString, FInDistanceAICharacterVariableHolder> InDistanceAICharacterVariables;

	
};
