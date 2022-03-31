// Fill out your copyright notice in the Description page of Project Settings.


#include "StreamBaseLevelScriptActor.h"
#include "Kismet/GameplayStatics.h"
#include "Weapon.h"


void AStreamBaseLevelScriptActor::BeginPlay()
{
	Super::BeginPlay();

	

	UGameplayStatics::GetAllActorsOfClass(GetLevel(), AAI_Character::StaticClass(), LevelAI);

	LoadLevelVariables();
}

void AStreamBaseLevelScriptActor::LoadLevelVariables()
{
	PlayerCharacterForLevelStreaming = CastChecked<APlayerCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));

	FText LevelLoadGameSlotName = PlayerCharacterForLevelStreaming->LoadGameSlotName;

	//Load the saved Game into saved game instance variable
//stops LoadGameSlot from crashing game when not set. This function gets called on beginplay on the level blueprint. If no save file then crash without this if
	if (LevelLoadGameSlotName.IsEmpty())
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, FString::Printf(TEXT("LOAD GAME SLOT NAME IS EMPTY in streaming level, SHOULD NEVER HAPPEN")));
		}
	}
	else
	{
		USaveGameM* SaveGameInstance = Cast<USaveGameM>(UGameplayStatics::CreateSaveGameObject(USaveGameM::StaticClass()));
		SaveGameInstance = Cast<USaveGameM>(UGameplayStatics::LoadGameFromSlot(LevelLoadGameSlotName.ToString(), 0));
		HeartsGameMode = Cast<AHeartsGameModeBase>(GetWorld()->GetAuthGameMode());

		//if this is a new game, no variables need to be saved -- but there still needs to be a new game slot because level is loaded based on the LoadGameSlotName at every construction
		if (LevelLoadGameSlotName.ToString() != PlayerCharacterForLevelStreaming->NewGameSlotNameWithJargon.ToString())
		{
			SetLevelAIVariables();
		}

		//show game was loaded
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Green, FString::Printf(TEXT("Level WAS LOADED")));
		}
	}

}

void AStreamBaseLevelScriptActor::SetLevelAIVariables()
{
	PlayerCharacterForLevelStreaming = CastChecked<APlayerCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));

	FText LevelLoadGameSlotName = PlayerCharacterForLevelStreaming->LoadGameSlotName;

	USaveGameM* SaveGameInstance = Cast<USaveGameM>(UGameplayStatics::CreateSaveGameObject(USaveGameM::StaticClass()));
	SaveGameInstance = Cast<USaveGameM>(UGameplayStatics::LoadGameFromSlot(LevelLoadGameSlotName.ToString(), 0));

	for (AActor* AIActor : LevelAI)
	{
		AAI_Character* CurrentAICharacterToRecieveVariables = CastChecked<AAI_Character>(AIActor);

		FInDistanceAICharacterVariableHolder CurrentAIVariables;

		FString CurrentAICharacterName = CurrentAICharacterToRecieveVariables->GetName();

		if (SaveGameInstance->InDistanceAICharacterVariables.Contains(CurrentAICharacterName))
		{
			CurrentAIVariables = SaveGameInstance->InDistanceAICharacterVariables[CurrentAICharacterName];

			CurrentAICharacterToRecieveVariables->SetActorLocation(CurrentAIVariables.AILocation);
			CurrentAICharacterToRecieveVariables->GetController()->SetControlRotation(CurrentAIVariables.AIControllerRotation);
			CurrentAICharacterToRecieveVariables->AICharacterStatSheet->Health = CurrentAIVariables.AIHealth;
			

			FString ForLogging = CurrentAICharacterToRecieveVariables->GetName();

			if (CurrentAIVariables.bIsAIDead)
			{
				CurrentAICharacterToRecieveVariables->bAIIsDead = true;
				CurrentAICharacterToRecieveVariables->TurnOff();
				CurrentAICharacterToRecieveVariables->Destroy();
				CurrentAICharacterToRecieveVariables->AIWeapon->Destroy();
				
				GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Orange, FString::Printf(TEXT("is dead %s"), *CurrentAICharacterName));
				
			}
		}
		else
		{
			float uniqueidforlogging = CurrentAICharacterToRecieveVariables->UniqueID;
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Orange, FString::Printf(TEXT("AI CHARACTER UNIQUE ID NOT FOUND IN ARRAY!. SHOULDNT HAPPEN! ---- stream %s"), *CurrentAICharacterName));
			}

		}


	}

	//clear TMap for next save
	SaveGameInstance->InDistanceAICharacterVariables.Empty();

}
