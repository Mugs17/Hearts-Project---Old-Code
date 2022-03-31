// Fill out your copyright notice in the Description page of Project Settings.


#include "AICharacterController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AI_Character.h"
#include "Engine/World.h"
#include "Waypoint.h"
#include "Kismet/KismetMathLibrary.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h" // prob can delete maybe
#include "PlayerCharacter.h" // idk if need either
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISense.h"
#include "Perception/AISenseConfig_Sight.h"




AAICharacterController::AAICharacterController()
{
	PrimaryActorTick.bCanEverTick = true;


}


void AAICharacterController::BeginPlay()
{
	Super::BeginPlay();

	AICharacter = Cast<AAI_Character>(GetPawn());


	if (AICharacter != nullptr) {
		UE_LOG(LogTemp, Warning, TEXT("Pawn is set for AICharacter"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Pawn wasnt set, idk why this would happen. Look into it."));
	}

	if (GetPerceptionComponent() != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Perception System Set"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Perception Not Set Problem Occured"));
	}


	SettingEnemyTagsInitial(GetPawn());


	GetWorldTimerManager().SetTimer(TimerHandle, this, &AAICharacterController::MyTimerFunction, 3.0f, true);

}

void AAICharacterController::OnPossess(APawn* PawnII)
{  
	Super::OnPossess(PawnII);

	
}

void AAICharacterController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	 

	if (AICharacter->bAIIsFightingC)
	{
		if (AICharacter->bAIsTurnToAttackC)
		{
			if (GetWorld()->GetTimeSeconds() >= AICharacter->DeltaTimeAITurnStartC + AICharacter->FightRoundTimeC)
			{
				AICharacter->bAIsTurnToAttackC = false;
				AICharacter->bEnemysTurnToAttackC = true;
				AICharacter->DeltaTimeEnemyTurnStartC = GetWorld()->GetTimeSeconds();
				UE_LOG(LogTemp, Error, TEXT("ENEMY TURN START FROM AI"));
			}

			// StartAttacking
			if (!AICharacter->bAIHasAttackedC && AICharacter->GetDistanceTo(ClosestEnemyActor) <= 200.0f && bIsMontageActive == false)
			{
				
			//	UE_LOG(LogTemp, Error, TEXT("Montage has to be set to False before this can be called."));

				//setting bool to true by default so unless AIAlreadyFighting is called and changes it to false, AICharacter will always continue its attack
				bool bCanAIContinueItsAttack = true;

				//saving copy of closest enemy actor for enemy change check
				if (ClosestEnemyActorCopy == nullptr)
				{
					ClosestEnemyActorCopy = ClosestEnemyActor;
				}
				else if (ClosestEnemyActorCopy && ClosestEnemyActor && ClosestEnemyActorCopy->GetUniqueID() != ClosestEnemyActor->GetUniqueID())
				{
					bCanAIContinueItsAttack = AIAlreadyFightingStartFightNewEnemyActor();
					//updating the copy to be current once the switch Enenemy and restart fight has already been completed
					ClosestEnemyActorCopy = ClosestEnemyActor;
				}

			
				if (bCanAIContinueItsAttack)
				{
					AIAttackEnemy(ClosestEnemyActor);   
					AICharacter->bAIHasAttackedC = true;
					//	UE_LOG(LogTemp, Error, TEXT("Montage Set To Active"));SS
					bIsMontageActive = true;
					//	UE_LOG(LogTemp, Error, TEXT("AIStarted Attack"));
				}

			}

		}
		else if (AICharacter->bEnemysTurnToAttackC)
		{
			if (GetWorld()->GetTimeSeconds()  >= AICharacter->DeltaTimeEnemyTurnStartC + AICharacter->FightRoundTimeC)
			{
				
				AICharacter->bEnemysTurnToAttackC = false;
				AICharacter->bAIsTurnToAttackC = true;
				AICharacter->DeltaTimeAITurnStartC = GetWorld()->GetTimeSeconds();
				UE_LOG(LogTemp, Error, TEXT("AI TURN START FROM AI"));

				// Clearing in preparation for AI turn
				AICharacter->bAIHasAttackedC = false;
			}

			// StartAttacking

		}
	} 


	if (bIsCharacterDetected && ClosestEnemyActor != nullptr)
	{
		if (!bIsMontageActive)
		{
			MoveToActor(ClosestEnemyActor, 100.0f);
		}

		if (!bIsFocusSet)
		{
			SetFocus(ClosestEnemyActor, EAIFocusPriority::Gameplay);
			bIsFocusSet = true;
		}

		if (!bIsRunning)
		{
			bIsRunning = true;
			AICharacter->AIToggleRun(bIsRunning);
		}
		

		
		// if fight hasnt started yet
		if (!AICharacter->bAIIsFightingC && AICharacter->GetDistanceTo(ClosestEnemyActor) <= 200.0f && bIsMontageActive == false)
		{
			StartFightNewEnemyActor();
		}


		if (bIsMontageActive == true && !AICharacter->IsAttackAnimMontagePlaying())
		{
			bIsMontageActive = false;
			
		}
	
	}






	else if (AICharacter->NextWaypoint != nullptr)
	{

		 MoveToActor(AICharacter->NextWaypoint, 5.0f);
		// UE_LOG(LogTemp, Warning, TEXT("Moving To Closest WAYPOINT"));

		 if (bIsFocusSet)
		 {
			 ClearFocus(EAIFocusPriority::Gameplay);
			 bIsFocusSet = false;
		 }

		 //AICharacter->GetName.
		 if (bIsRunning)
		 {
			 bIsRunning = false;
			 AICharacter->AIToggleRun(bIsRunning);
		 }
	}

	


}




void AAICharacterController::SetNewClosestActor(AActor* NewActor)
{
	ClosestEnemyActor = NewActor;

	//setting focus to false so focus can be reset

	ClearFocus(EAIFocusPriority::Gameplay);
	bIsFocusSet = false;
}

void AAICharacterController::PickingNewClosestActor(AActor* Actor2)
{
	// Checks to see if actor tag matches enemy tag list item

	bool ShouldIAttack = Should_AI_Attack(Actor2);


	if (ShouldIAttack)
	{
		if (ClosestEnemyActor == nullptr)
		{
			SetNewClosestActor(Actor2);
			//	UE_LOG(LogTemp, Warning, TEXT("Closest Actor Set Early -- shouldnt happen without bool check"));
			bIsCharacterDetected = true;
		}
		//Making Sure ClosestActor and Actor isn't the same -- might not be necessary bc GetDistanceTo < GetDistanceTo should disprove anyways
		else if (ClosestEnemyActor->GetUniqueID() != Actor2->GetUniqueID())
		{
			DistanceToCurrentArrayActor = AICharacter->GetDistanceTo(Actor2);
			//Updating distance to closest Actor so it doesn't only go lower
			DistanceToClosestActor = AICharacter->GetDistanceTo(ClosestEnemyActor);
			if (DistanceToCurrentArrayActor < DistanceToClosestActor)
			{
				DistanceToClosestActor = DistanceToCurrentArrayActor;
				SetNewClosestActor(Actor2);
				//	UE_LOG(LogTemp, Warning, TEXT("Closest Actor Set -- shouldnt happen without bool check logs"));
				bIsCharacterDetected = true;
			}
		}
	}
}

void AAICharacterController::StartFightNewEnemyActor()
{
	EnemyAICharacter = Cast<AAI_Character>(ClosestEnemyActor);
	//FOR ENEMY 
	if (EnemyAICharacter)
	{
		if (!EnemyAICharacter->bAIIsFightingC)
		{
			UE_LOG(LogTemp, Error, TEXT("ENEMY CHARACTER IS NOT FIGHITNG SO ENGAGING"));
			// need to set start timer for both enemy AI and self AI
			AICharacter->DeltaTimeAITurnStartC = GetWorld()->GetTimeSeconds();
			EnemyAICharacter->DeltaTimeEnemyTurnStartC = AICharacter->DeltaTimeAITurnStartC;

			AICharacter->bAIsTurnToAttackC = true;
			AICharacter->bEnemysTurnToAttackC = false;

			EnemyAICharacter->bAIsTurnToAttackC = false;
			EnemyAICharacter->bEnemysTurnToAttackC = true;

			//need to set both AI to fighting bc both are inactive
			AICharacter->bAIIsFightingC = true;
			EnemyAICharacter->bAIIsFightingC = true;
			AICharacter->bAIHasAttackedC = false;


		}
		else if (EnemyAICharacter->bAIIsFightingC)
		{
			UE_LOG(LogTemp, Error, TEXT("ENEMY CHARACTER IS FIGHITNG"));
			if (EnemyAICharacter->bAIsTurnToAttackC)
			{
				//setting enemies turn to attack to true, and enemies turn start timer
				AICharacter->DeltaTimeEnemyTurnStartC = EnemyAICharacter->DeltaTimeAITurnStartC;
				AICharacter->bEnemysTurnToAttackC = true;

				// setting AIs turn to attack to false
				AICharacter->bAIsTurnToAttackC = false;

				//setting this AI is fighing to true so it can fight
				AICharacter->bAIIsFightingC = true;

			}
			else if (EnemyAICharacter->bEnemysTurnToAttackC)
			{
				//Setting turn to my turn
				AICharacter->DeltaTimeAITurnStartC = EnemyAICharacter->DeltaTimeEnemyTurnStartC;
				AICharacter->bAIsTurnToAttackC = true;
				//setting enemy turn to false
				AICharacter->bEnemysTurnToAttackC = false;

				//setting this AI is fighing to true so it can fight
				AICharacter->bAIIsFightingC = true;
			}
		}


	}
	//FOR PLAYER CHARACTER ENEMY
	else if (ClosestEnemyActor->ActorHasTag(TEXT("PlayerCharacter")))
	{
		PlayerCharacter = Cast<APlayerCharacter>(ClosestEnemyActor);
		if (PlayerCharacter)
		{
			if (!PlayerCharacter->bPlayerIsFighting)
			{
				//add might need to be before bc tick on player could startFight b4 this is added.
				//pretty sure i only need to add this if player is inactive / not fighting like its being done here
				PlayerCharacter->AddHaveAttackedAICharacterToPlayer(AICharacter);

				PlayerCharacter->bPlayersTurnToAttack = false;
				PlayerCharacter->bEnemysTurnToAttack = true;

				AICharacter->bEnemysTurnToAttackC = false;
				AICharacter->bAIsTurnToAttackC = true;

				AICharacter->DeltaTimeAITurnStartC = GetWorld()->GetTimeSeconds();
				PlayerCharacter->DeltaTimeEnemyTurnStart = AICharacter->DeltaTimeAITurnStartC;

				PlayerCharacter->StopPlayerMovementAndRotationControl(AICharacter);

				AICharacter->bAIIsFightingC = true;
				PlayerCharacter->bPlayerIsFighting = true;
			}
			else if (PlayerCharacter->bPlayerIsFighting)
			{
				if (PlayerCharacter->bPlayersTurnToAttack)
				{

					//Setting to enemies turn
					AICharacter->DeltaTimeEnemyTurnStartC = PlayerCharacter->DeltaTimePlayerTurnStart;
					AICharacter->bEnemysTurnToAttackC = true;

					//setting AIs turn to false
					AICharacter->bAIsTurnToAttackC = false;

					//setting this AI is fighing to true so it can fight
					AICharacter->bAIIsFightingC = true;
				}
				else if (PlayerCharacter->bEnemysTurnToAttack)
				{
					//Setting to AIs turn
					AICharacter->DeltaTimeAITurnStartC = PlayerCharacter->DeltaTimeEnemyTurnStart;
					AICharacter->bAIsTurnToAttackC = true;

					//setting Enemiess turn to false
					AICharacter->bEnemysTurnToAttackC = false;
 
					//setting this AI is fighing to true so it can fight
					AICharacter->bAIIsFightingC = true;
				}
			}

		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("CLOSEST ACTOR NOT AI OR PLAYER --- make sure u know why this is happening"));
	}
}

// BOOL is for if I can finish my turn(true), or cancel attack and start EnemyTurn (false)
bool AAICharacterController::AIAlreadyFightingStartFightNewEnemyActor()
{
	UE_LOG(LogTemp, Warning, TEXT("AI ALREADY FIGHTING Start fight with new closest enemy actor IS BEING CALLED"))
	EnemyAICharacter = Cast<AAI_Character>(ClosestEnemyActor);
	//FOR ENEMY 
	if (EnemyAICharacter)
	{
		if (!EnemyAICharacter->bAIIsFightingC)
		{
			UE_LOG(LogTemp, Error, TEXT("ENEMY CHARACTER IS NOT FIGHITNG SO ENGAGING"));
			// need to set start timer for just EnemyAI because its already my turn so my timer is already set
			EnemyAICharacter->DeltaTimeEnemyTurnStartC = AICharacter->DeltaTimeAITurnStartC;

			//redundant bc to be attacking this already has to be set but probably won't hurt but uncommenting bc unecessary 
//			AICharacter->bAIsTurnToAttackC = true;
//			AICharacter->bEnemysTurnToAttackC = false;

			EnemyAICharacter->bAIsTurnToAttackC = false;
			EnemyAICharacter->bEnemysTurnToAttackC = true;

			//need to set EnemyAI to Fighting bc its not fighting
			EnemyAICharacter->bAIIsFightingC = true;

			//Can stay attacking
			return true;

		}
		else if (EnemyAICharacter->bAIIsFightingC)
		{
			//if fighting I want to switch battle timing to enemy so this can all be the same as the normal AIStartFight function
			UE_LOG(LogTemp, Error, TEXT("ENEMY CHARACTER IS FIGHITNG"));
			if (EnemyAICharacter->bAIsTurnToAttackC)
			{
				//setting enemies turn to attack to true, and enemies turn start timer
				AICharacter->DeltaTimeEnemyTurnStartC = EnemyAICharacter->DeltaTimeAITurnStartC;
				AICharacter->bEnemysTurnToAttackC = true;

				// setting AIs turn to attack to false
				AICharacter->bAIsTurnToAttackC = false;

				//can't continue my turn
				return false;
			}
			else if (EnemyAICharacter->bEnemysTurnToAttackC)
			{
				//Resetting AI timer to match Enemies timer
				AICharacter->DeltaTimeAITurnStartC = EnemyAICharacter->DeltaTimeEnemyTurnStartC;

				//can continue my turn
				return true;
			}
			//shouldn't ever be called bc returning true in if statements aboce
			return false;
		}

		//shouldn't ever be called bc returning true in if statements aboce
		return false;
	}
	//FOR PLAYER CHARACTER ENEMY
	else if (ClosestEnemyActor->ActorHasTag(TEXT("PlayerCharacter")))
	{
		PlayerCharacter = Cast<APlayerCharacter>(ClosestEnemyActor);
		if (PlayerCharacter)
		{
			if (!PlayerCharacter->bPlayerIsFighting)
			{
				//add might need to be before bc tick on player could startFight b4 this is added.
				//pretty sure i only need to add this if player is inactive / not fighting like its being done here
				PlayerCharacter->AddHaveAttackedAICharacterToPlayer(AICharacter);

				PlayerCharacter->bPlayersTurnToAttack = false;
				PlayerCharacter->bEnemysTurnToAttack = true;

				AICharacter->bEnemysTurnToAttackC = false;
				AICharacter->bAIsTurnToAttackC = true;

				//My already in place stop timer as this is happening inside my attack
				PlayerCharacter->DeltaTimeEnemyTurnStart = AICharacter->DeltaTimeAITurnStartC;

				PlayerCharacter->StopPlayerMovementAndRotationControl(AICharacter);

				PlayerCharacter->bPlayerIsFighting = true;

				//can continue my turn
				return true;
			}
			else if (PlayerCharacter->bPlayerIsFighting)
			{
				if (PlayerCharacter->bPlayersTurnToAttack)
				{

					//Setting to enemies turn
					AICharacter->DeltaTimeEnemyTurnStartC = PlayerCharacter->DeltaTimePlayerTurnStart;
					AICharacter->bEnemysTurnToAttackC = true;

					//setting AIs turn to false
					AICharacter->bAIsTurnToAttackC = false;

					//can't continue my turn as it is players turn to attack
					return false;

				}
				else if (PlayerCharacter->bEnemysTurnToAttack)
				{
					//Syncing my turn start to players Enemy turn start
					AICharacter->DeltaTimeAITurnStartC = PlayerCharacter->DeltaTimeEnemyTurnStart;

					// can continue my attack
					return true;
				}
				//shouldn't ever be called bc returning true in if statements above
				return false;
			}
		//shouldn't ever be called bc returning true in if statements above
			return false;
		}
		//shouldn't ever be called bc returning true in if statements above
		return false;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("CLOSEST ACTOR NOT AI OR PLAYER --- make sure u know why this is happening"));
		//random value sent
		return false;
	}
}






 
FRotator AAICharacterController::GetControlRotation() const
{

	if (GetPawn() == nullptr)
	{
		return FRotator(0.0f, 0.0f, 0.0f);
	}


	return FRotator(0.0f, GetPawn()->GetActorRotation().Yaw, 0.0f);
}

void AAICharacterController::OnPawnDetected(const TArray<AActor*>& DetectedPawns)
{

}






void AAICharacterController::MyTimerFunction()
{

	if (AICharacter->AISphereCollisionFieldActorsC.Num() == 0)
	{
		ClearFocus(EAIFocusPriority::Gameplay);
		bIsCharacterDetected = false;
		ClosestEnemyActor = nullptr;
		DistanceToClosestActor = 10000000;

		UE_LOG(LogTemp, Warning, TEXT("Currently PercievedActorsArray is empty ----"));

		//Stop AI from moving if it doesnt have a waypoint to go to -- should prob just set a waitpoint for still characters so they return to starting location

		if (AICharacter->NextWaypoint == nullptr)
		{
			StopMovement();
		}
	}
	else
	{
	
		/// Setting ClosestActor for movement and Checking to see if their is a character to move to. Else will move to waypoint on tick
		for (AActor* Actor : AICharacter->AISphereCollisionFieldActorsC /*array*/)
		{	

			AAI_Character* CurrentArrayActor = Cast<AAI_Character>(Actor); 

			if (CurrentArrayActor)
			{
				// better to do a was recently fighting check that gets set to false on a timer
				//if (CurrentArrayActor->bAIIsFightingC)
				//{
			//		UE_LOG(LogTemp, Error, TEXT("AI IS BEING FOUND"));
					PickingNewClosestActor(Actor);
				//}
			}
			else if (APlayerCharacter* CurrentArrayPlayerCharacter = Cast<APlayerCharacter>(Actor))
			{
				//if (CurrentArrayPlayerCharacter->bPlayerIsFighting)
			//	{
					UE_LOG(LogTemp, Error, TEXT("Player IS BEING FOUND"));
					PickingNewClosestActor(Actor);
				//}
			}
			
		}
	}

	//UE_LOG(LogTemp, Warning, TEXT("Number of Actors in Array = %d"), CurrentlyPercievedActorsArray.Num());

}
 

//Could add to ReturningOwningAI_Tags and prob should for performance but this is clearer
void AAICharacterController::SettingEnemyTagsInitial(AActor* OwningActor)
{
	for (FName Tag : OwningActor->Tags)
	{
		if (Tag == FName("FriendlyNPC"))
		{
			 EnemyTags.Add(FName("EnemyNPC"));
		} 
		else if (Tag == FName("EnemyNPC"))
		{
			EnemyTags.Add(FName("FriendlyNPC"));
	
			EnemyTags.Add(FName("PlayerCharacter"));
	
		} 
	}
}
// Checks to see if OtherCharacter has one of its EnemyTags, and if so, returns true / yes
bool AAICharacterController::Should_AI_Attack(AActor* OtherCharacter)
{
	for (FName OtherTag : OtherCharacter->Tags)
	{
		for (FName Tag : EnemyTags)  
		{
			if (OtherTag == Tag)
			{
				return true;
			}
		}
	}

	return false;
}

void AAICharacterController::AIAttackEnemy(AActor* EnemyActor)
{
	// the AI attack function starts at the attack location 
	if (EnemyActor)
	{
		AICharacter->GetMovementComponent()->StopActiveMovement();
		if (AICharacter->SwordAttackAnimation)
		{
			AICharacter->PlayAnimMontage(AICharacter->SwordAttackAnimation);

			float EnemyHealthValueForLogging;
			float PlayerEnemyHealthValueForLogging;

			AIEnemyActorForDeathTimer = Cast<AAI_Character>(EnemyActor);

			PlayerEnemyActorForDeathTimer = Cast<APlayerCharacter>(EnemyActor);

			if (AIEnemyActorForDeathTimer)
			{

				//resetting play enemy in case it was set before
				PlayerEnemyActorForDeathTimer = nullptr;

				EnemyHealthValueForLogging = AIEnemyActorForDeathTimer->AICharacterStatSheet->Health - AICharacter->AICharacterStatSheet->Damage;
				AIEnemyActorForDeathTimer->AICharacterStatSheet->Health = EnemyHealthValueForLogging;

				//Putting extra if outside of next if statement because bAIIsDead being made false by different AI will lead to ClosestEnemyAI to never be cleared
				if (EnemyHealthValueForLogging <= 0)
				{
					//resetting ClosestEnemyActor so the AI doesn't think dead enemy still exists
					ClosestEnemyActor = nullptr;

					//Ending Fight so the fight loop doesnt keep playing
					AICharacter->bAIIsFightingC = false;
				}

				if (EnemyHealthValueForLogging <= 0 && AIEnemyActorForDeathTimer->bAIIsDead == false)
				{
					KillEnemyAIStopCurrentFight(AIEnemyActorForDeathTimer);

				}

				else if (GEngine)
				{
					//GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, FString::Printf(TEXT("EnemyAI of AI Health = %f"), EnemyHealthValueForLogging));

					//Setting Enemy Health
					AIEnemyActorForDeathTimer->AICharacterStatSheet->Health = EnemyHealthValueForLogging;
				}
			}
			else if (PlayerEnemyActorForDeathTimer)
			{
				// resetting AI Enemy Actor in case it was set before so the timer function doesn't call it in if statement
				AIEnemyActorForDeathTimer = nullptr;

				PlayerEnemyHealthValueForLogging = PlayerEnemyActorForDeathTimer->PlayerCharacterSheet->Health - AICharacter->AICharacterStatSheet->Damage;
				PlayerEnemyActorForDeathTimer->PlayerCharacterSheet->Health = PlayerEnemyHealthValueForLogging;

				if (PlayerEnemyActorForDeathTimer->PlayerCharacterSheet->Health <= 0)
				{
					//resetting ClosestEnemyActor so the AI doesn't think dead enemy still exists
					ClosestEnemyActor = nullptr;

					//Ending Fight so the fight loop doesnt keep playing
					AICharacter->bAIIsFightingC = false;
				}

				if (PlayerEnemyHealthValueForLogging <= 0 && PlayerEnemyActorForDeathTimer->bPlayerIsDead == false)
				{
						
					KillEnemyPlayerStopCurrentFight(PlayerEnemyActorForDeathTimer);
				}

				else if (GEngine)
				{
					GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Yellow, FString::Printf(TEXT("Player Enemy of AI Health = %f"), PlayerEnemyHealthValueForLogging));

					//Setting Enemy Health
					PlayerEnemyActorForDeathTimer->PlayerCharacterSheet->Health = PlayerEnemyHealthValueForLogging;
				}
			}

			UE_LOG(LogTemp, Warning, TEXT("Montage Called to Play and Movement Should be stopped"));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("AttackAnimation not found!"));
		}
	}

}

void AAICharacterController::KillEnemyAIStopCurrentFight(AAI_Character* EnemyAIForDeathHandling)
{

	EnemyAIForDeathHandling->bAIIsDead = true;


	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, FString::Printf(TEXT("Enemy of AI is dead")));
	}



	//	AIEnemyActor->PlayAIDeathAnimation();

	EnemyAIForDeathHandling->GetController()->Destroy();
	//EnemyAIForDeathHandling->SetActorEnableCollision(ECollisionEnabled::NoCollision);

	//Change EnemyActor from using animation instance to a singular animation asset for death animation using Uskeletal Mesh Component
	//EnemyAIForDeathHandling->AICharacterMesh->SetAnimationMode(EAnimationMode::AnimationSingleNode);
	if (EnemyAIForDeathHandling->DeathAnimationMontage)
	{
		EnemyAIForDeathHandling->AICharacterMesh->PlayAnimation(EnemyAIForDeathHandling->DeathAnimationMontage, false);
		float DeathAnimationLength = EnemyAIForDeathHandling->DeathAnimationMontage->GetPlayLength();

		EnemyAIForDeathHandling->bAIIsDead = true;
		GetWorldTimerManager().SetTimer(EnemyAIForDeathHandling->DeathTimerHandle, EnemyAIForDeathHandling, &AAI_Character::AIDeathTimerFunction, DeathAnimationLength - 0.4f, false);
	}




	// need to stop animation component somehow
}

void AAICharacterController::KillEnemyPlayerStopCurrentFight(APlayerCharacter* EnemyPlayerForDeathHandling)
{

	EnemyPlayerForDeathHandling->bPlayerIsDead = true;


	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, FString::Printf(TEXT("Enemy of AI is dead")));
	}



	//	AIEnemyActor->PlayAIDeathAnimation();
	EnemyPlayerForDeathHandling->SetActorEnableCollision(ECollisionEnabled::NoCollision);

	//Change EnemyActor from using animation instance to a singular animation asset for death animation using Uskeletal Mesh Component
	EnemyPlayerForDeathHandling->CharacterMesh->SetAnimationMode(EAnimationMode::AnimationSingleNode);
	if (EnemyPlayerForDeathHandling->DeathAnimMontage)
	{
		EnemyPlayerForDeathHandling->CharacterMesh->PlayAnimation(EnemyPlayerForDeathHandling->DeathAnimMontage, false);
		float DeathAnimationLength = EnemyPlayerForDeathHandling->DeathAnimMontage->GetPlayLength();

		EnemyPlayerForDeathHandling->bPlayerIsDead = true;
		GetWorldTimerManager().SetTimer(EnemyPlayerForDeathHandling->PlayerDeathTimerHandle, EnemyPlayerForDeathHandling, &APlayerCharacter::PlayerDeathTimerFunction, DeathAnimationLength - 0.4f, false);
	}




	// need to stop animation component somehow
}







