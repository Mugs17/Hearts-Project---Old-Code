  // Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Components/CapsuleComponent.h"
#include "PlayerCharacter.h"
#include "AI_Character.h"
#include "AICharacterController.generated.h"

/**
 * 
 */


UCLASS()
class HEARTS_API AAICharacterController : public AAIController
{
	GENERATED_BODY()

public:

		AAICharacterController();

		virtual void BeginPlay() override;

		virtual void OnPossess(APawn* PawnII) override;

		virtual void Tick(float DeltaSeconds) override;

		virtual FRotator GetControlRotation() const override;

		AAI_Character* AICharacter;

		AAI_Character* EnemyAICharacter;

		AController* EnemyAIController;

		APlayerCharacter* PlayerCharacter;

		
		UFUNCTION()
		void OnPawnDetected(const TArray<AActor*>& DetectedPawns);

		UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = AI)
			float AISightRadius = 800.0f;

		UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = AI)
			float AISightAge = 5.0f;

		UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = AI)
			float AILoseSightRadius = AISightRadius + 5.0f;

		UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = AI)
			float AIFieldOfView = 180.0f;

		UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = AI)
			class UAISenseConfig_Sight* SightConfig;

		UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = AI)
			bool bIsCharacterDetected = false;

		float DistanceToCurrentArrayActor = 100000000;
		float DistanceToClosestActor = 10000000;

		AActor* ClosestEnemyActor; 

		//Used to check if actor has changed
		AActor* ClosestEnemyActorCopy;

		// For The Timer

		void MyTimerFunction();

		FTimerHandle TimerHandle;

		//its file global bc of the death timer but is used to deal damage
		AAI_Character* AIEnemyActorForDeathTimer;

		//its file global bc of the death timer but is used to deal damage
		APlayerCharacter* PlayerEnemyActorForDeathTimer;



// For Tags

		TArray<FName> AI_Tags;

		TArray<FName> EnemyTags;

		UFUNCTION()
		void SettingEnemyTagsInitial(AActor* OwningActor);

		UFUNCTION()
		bool Should_AI_Attack(AActor* EnemyPawn);

// for movement 

		bool bIsRunning = false;

		bool bIsFocusSet = false;

		void SetNewClosestActor(AActor* NewActor);

		//Fighting

		//bool bAIIsFighting = false;

	//	bool bStartFight = false;

	//	bool bAIsTurnToAttack = false;

	//	bool bEnemysTurnToAttack = false;

	//	bool bAIHasAttacked = false;


	//	bool bIsAIInstigator;

		
		//montage

		bool bIsMontageActive = false;

		// for enemy tag checking

	//	FName EnemyTag;

		// For Distance Check / Setting ClosestActor

		void PickingNewClosestActor(AActor* Actor2);


		// for collision blocking wall

		// for engaging enemyActor

		void StartFightNewEnemyActor();
		bool AIAlreadyFightingStartFightNewEnemyActor();

		//attack enemy

		void AIAttackEnemy(AActor* EnemyActor);

		void KillEnemyAIStopCurrentFight(AAI_Character* EnemyAIForDeathHandling);

		void KillEnemyPlayerStopCurrentFight(APlayerCharacter* EnemyPlayerForDeathHandling);

		 









 

};
