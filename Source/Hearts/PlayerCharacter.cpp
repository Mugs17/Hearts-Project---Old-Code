               // Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"
#include "Animation/SkeletalMeshActor.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Weapon.h"

  
// Sets default values
APlayerCharacter::APlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	ActiveTraceLineComponent = CreateDefaultSubobject<UViewPointLine>(TEXT("TraceLineForActive"));

	PlayerCharacterSheet = CreateDefaultSubobject<UCharacterStatisticsComponent>(TEXT("Player Statistics Sheet"));

	//ActiveWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("ActiveImageWidget"));
	//ActiveWidgetComponent->SetupAttachment(RootComponent);

	//MyCollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere Volume"));
	//MyCollisionSphere->InitSphereRadius(1000.0f);
	//MyCollisionSphere->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("CollisionSphereSocket"));
	//MyCollisionSphere->ShapeColor = FColor::Green;
	//MyCollisionSphere->SetVisibility(true);
	//MyCollisionSphere->SetIsReplicated(true);

	CollisionFieldActors.Reserve(600);
   

//	GetCharacterMovement()->
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
//	SaveNewGameSlot();


	if (GEngine)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Orange, FString::Printf(TEXT("AICharacter of name: %s was set +++++"), *test));
	}

	Weapon = GetWorld()->SpawnActor<AWeapon>(WeaponClass);
	Weapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("WeaponSocketR"));
	Weapon->SetOwner(this);

 
	MyCollisionSphere = NewObject<USphereComponent>(this);
	MyCollisionSphere->SetupAttachment(this->GetRootComponent(), TEXT("CollisionSphereSocket"));
	MyCollisionSphere->RegisterComponent();

	MyCollisionSphere->InitSphereRadius(5.0f);
	

	MyCollisionSphere->ShapeColor = FColor::Green;
	MyCollisionSphere->SetIsReplicated(true);
	MyCollisionSphere->SetVisibility(true);
	MyCollisionSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	//MyCollisionSphere->SetCollisionObjectType(ECC_GameTraceChannel2)
	MyCollisionSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	MyCollisionSphere->SetCollisionResponseToChannel(ECC_GameTraceChannel3, ECR_Overlap);
	//trying to attemp line trace collision
	//MyCollisionSphere->Trace
	MyCollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &APlayerCharacter::OnOverlapBegin);
	MyCollisionSphere->OnComponentEndOverlap.AddDynamic(this, &APlayerCharacter::OnOverlapEnd);

	MyCollisionSphere->SetSphereRadius(10000.0f);


	//Check Actors in CollisionFieldActors Array

	GetWorldTimerManager().SetTimer(TimerHandle, this, &APlayerCharacter::MyTimerFunction, 0.2f, true);

	CharacterMesh = GetMesh();

	PlayerController = this->GetController(); 
	if (!PlayerController)
	{
		UE_LOG(LogTemp, Error, TEXT("Player controller not found should not be happening"));
	}

	CharacterAnimInstance = (CharacterMesh) ? CharacterMesh->GetAnimInstance() : nullptr;

	if (!CharacterAnimInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("AICharacterInstance and Mesh not beign set in constructor"));
	}


	
}
 
// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	if (AIEnemyForAttackDistanceCheck)
	{
		if (this->GetDistanceTo(AIEnemyForAttackDistanceCheck) <= 200.0f)
		{
			UE_LOG(LogTemp, Display, TEXT("Player should be attacking now"));
			//Stop movement
			GetMovementComponent()->StopActiveMovement();

			//play animation
			PlayerPlayAttackAnimation();

			//setting to nullptr so doesnt get called again. Re-set only during AttackEnemyAuto function
			AIEnemyForAttackDistanceCheck = nullptr;
		}
	}

	if (bPlayerIsFighting)
	{
		if (bPlayersTurnToAttack)
		{

			// THIS IS BEING CALLED.. inside function attack is not
			if (GetWorld()->GetTimeSeconds() >= DeltaTimePlayerTurnStart + FightRoundTime)
			{

				bPlayersTurnToAttack = false;
				UE_LOG(LogTemp, Display, TEXT("player turn to attack = false  -- player turn to attack"));
				bEnemysTurnToAttack = true;
				UE_LOG(LogTemp, Display, TEXT("enemy turn to attack = true -- player turn to attack"));
				//might need extra playerhasattacked = false, bc if engaging other enemy, player has attacked might never be called
				DeltaTimeEnemyTurnStart = GetWorld()->GetTimeSeconds();
				UE_LOG(LogTemp, Error, TEXT("ENEMY TURN(On player.cpp) START"));
			}

			if (bIsMontageActive)
			{
				if (!bIsAttackAnimMontagePlaying())
				{ 
					bIsMontageActive = false;
				}

			}
			//Start Auto attack response
			if (bPlayerInactiveAndEnemyHasAttacked && !bPlayerHasAttacked && bIsMontageActive == false)
			{
				this->AttackEnemyAuto(PlayerInactiveFirstAttackedEnemy);
			//	UE_LOG(LogTemp, Warning, TEXT("Only Enemy should be being targetted, not closest actor"));
				bPlayerHasAttacked = true;
				bIsMontageActive = true;   
				UE_LOG(LogTemp, Display, TEXT("Attack function called"));
			}
			// StartAttacking
			//	else if (!bPlayerHasAttacked && this->GetDistanceTo(SelectedCharacter) <= 200.0f && bIsMontageActive == false)
			else if (!bPlayerInactiveAndEnemyHasAttacked &&!bPlayerHasAttacked && bIsMontageActive == false && ManuallySelectedEnemyCharacter)
			{

				this->AttackEnemyAuto(ManuallySelectedEnemyCharacter);
				UE_LOG(LogTemp, Display, TEXT("Attack function called2"));
				bPlayerHasAttacked = true;
				bIsMontageActive = true;

			}

		}
		else if (bEnemysTurnToAttack)
		{
			if (GetWorld()->GetTimeSeconds() >= DeltaTimeEnemyTurnStart + FightRoundTime)
			{

				bEnemysTurnToAttack = false;
				UE_LOG(LogTemp, Display, TEXT("Enemy turn to attack = false  -- enemy turn to attack"));
				bPlayersTurnToAttack = true;
				UE_LOG(LogTemp, Display, TEXT("Player turn to attack = true  -- enemy turn to attack"));
				DeltaTimePlayerTurnStart = GetWorld()->GetTimeSeconds();
				UE_LOG(LogTemp, Error, TEXT("Player TURN START"));

				// Clearing in preparation for AI turn-- THIS NEEDS TO GET SET TO FALSE FOR PLAYER TO GET ATTACKED AGAIN, enemy switch skips this, if i want to play attack immediately must add to engage functionality
				bPlayerHasAttacked = false;
			}


		}
		
	}


}

// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &APlayerCharacter::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &APlayerCharacter::MoveRight);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis(TEXT("LookRight"), this, &APawn::AddControllerYawInput);

	PlayerInputComponent->BindAction(TEXT("Jump"), EInputEvent::IE_Pressed, this, &APlayerCharacter::Jump);
	PlayerInputComponent->BindAction(TEXT("AttackRight"), EInputEvent::IE_Pressed, this, &APlayerCharacter::AttackRight);
	PlayerInputComponent->BindAction(TEXT("SHMouse"), EInputEvent::IE_Pressed, this, &APlayerCharacter::SHMouse);

	PlayerInputComponent->BindAction(TEXT("ToggleRun"), EInputEvent::IE_Pressed, this, &APlayerCharacter::ToggleRun);    

	PlayerInputComponent->BindAction(TEXT("EngageSelected"), EInputEvent::IE_Pressed, this, &APlayerCharacter::EngageSelection);

	PlayerInputComponent->BindAction(TEXT("CancelCombat"), EInputEvent::IE_Pressed, this, &APlayerCharacter::CancelCombat);

	PlayerInputComponent->BindAction(TEXT("FreeCamera"), EInputEvent::IE_Pressed, this, &APlayerCharacter::FreeCamera);

}

void APlayerCharacter::CancelCombat()
{
	if (bPlayerIsFighting)
	{
		bPlayerIsFighting = false;
		UE_LOG(LogTemp, Warning, TEXT("Player is fighting set to false on key press--"))
			// enable player rotation control
			StartPlayerMovementAndRotationControl();
	}
}
  
void APlayerCharacter::PlayPlayerDeathAnimation()
{
	if (DeathAnimMontage)
	{
		PlayAnimMontage(DeathAnimMontage);
	}
}

void APlayerCharacter::MoveForward(float AxisValue)
{
	if (bPlayerCanMove)
	{
		AddMovementInput(GetActorForwardVector() * AxisValue);
	}

}

void APlayerCharacter::MoveRight(float AxisValue)
{
	if (bPlayerCanMove)
	{
		AddMovementInput(GetActorRightVector() * AxisValue);
	}
}

void APlayerCharacter::LookUp(float AxisValue)
{
	if (!CameraIsFree)    
	{
		//AddControllerPitchInput(AxisValue);
		UE_LOG(LogTemp, Warning, TEXT("Controller Yaw being called"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("++++++"));
	}
//	UE_LOG(LogTemp, Warning, TEXT("LookUp is being called"));
	
}

void APlayerCharacter::LookRight(float AxisValue)
{
	//UE_LOG(LogTemp, Warning, TEXT("LookRight is being called"));
	if (!CameraIsFree)
	{
	//	AddControllerPitchInput(AxisValue);
		UE_LOG(LogTemp, Warning, TEXT("Controller Pitch being called"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("---------"));
	}
}

void APlayerCharacter::QuitGame()
{
	GetWorldTimerManager().SetTimer(QuitGameTimerHandle, this, &APlayerCharacter::QuitGameTimerFunction, 5.0, false);
}

void APlayerCharacter::Jump()
{
	ACharacter::Jump();
	if (JumpMontage)
	{
		UE_LOG(LogTemp, Warning, TEXT("Montage Is Set"));
		PlayAnimMontage(JumpMontage);
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("Montage called but not set"));
	}
}

void APlayerCharacter::AttackRight()
{
	if (AttackRightMontage)
	{
		UE_LOG(LogTemp, Warning, TEXT("Montage Is Set"));
		PlayAnimMontage(AttackRightMontage);
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("Montage called but not set"));
	}
}

void APlayerCharacter::SHMouse()
{
	APlayerController* MyController = GetWorld()->GetFirstPlayerController();

	if (MyController->bShowMouseCursor == false)
	{
		MyController->bShowMouseCursor = true;
		MyController->bEnableClickEvents = true;
		MyController->bEnableMouseOverEvents = true;
	}
	else if (MyController->bShowMouseCursor == true)
	{
		MyController->bShowMouseCursor = false;  
		MyController->bEnableClickEvents = false;
		MyController->bEnableMouseOverEvents = false;
	}

}

void APlayerCharacter::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("SUCCESFUL OVERLAP BEGIN for player character"));

	//If Returning Actor is Closest Actor -- im doing this bc no way to tell when ActorArray = nullptr // even if useless could stop bug if closest actor returns before timer function clears Closest Actor to nullptr;
	if (ClosestActor != nullptr && ClosestActor->GetUniqueID() == OtherActor->GetUniqueID())
	{
		AAI_Character* ClosestActorAIForWidget = Cast<AAI_Character>(OtherActor);
		if (ClosestActorAIForWidget != nullptr)
		{
			if (HitResultActorOfLineTrace == nullptr)
			{
				ClosestActorAIForWidget->SetActiveWidgetToVisible();  
				//ClosestActorForAIWidget is the selected actor bc the closest actor just re-entered the field so setting it to selected actor
				CurrentlySelectedCharacter = ClosestActorAIForWidget;
			}
			

		//	UE_LOG(LogTemp, Warning, TEXT("OVERLAPPED ACTOR SET TO VISIBLE BC ITS CLOSES ACTOR"));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("ClosestAIActor Cast to AI_Character is returning nullptr, shouldnt happen.. Find out why"));
		}
	}

	if (OtherActor != nullptr)
	{
		CollisionFieldActors.Add(OtherActor);
	//	UE_LOG(LogTemp, Warning, TEXT("OtherActor Added"));
	} 

}

void APlayerCharacter::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	UE_LOG(LogTemp, Warning, TEXT("SUCCESFUL OVERLAP END"));

	// Sets Widget off if Closest Actor is leaving the Collision Sphere ------- BC IT CANT BE SET TO HIDDEN in TIMER function
	if(ClosestActor != nullptr && OtherActor->GetUniqueID() == ClosestActor->GetUniqueID())
	{
		AAI_Character* AI_CharacterForWidget;

		AI_CharacterForWidget = Cast<AAI_Character>(OtherActor);

		if (AI_CharacterForWidget != nullptr)
		{
			AI_CharacterForWidget->SetActiveWidgetToHidden();
			// Set selected Character to nullptr bc no actors in range;
			CurrentlySelectedCharacter = nullptr;
		}

	}
	
	CollisionFieldActors.Remove(OtherActor);  
}

void APlayerCharacter::MyTimerFunction()
{

	//Check if LineTrace has hit an actor
	//if Line Trace has hit an actor I want to set ClosestActor widget to hidden and traced object widget to visible
	//I have to cast HitResultActor to AICharacter to set widget, so for performance reasons, I need to set a bool so this doesnt need to happen unecessarily

	HitResultActorOfLineTrace = ActiveTraceLineComponent->ReturnActorInLineTrace(); //perhaps can make this return a reference so it only needs to be called once and not on every timer call --- look into later

	// if extra time and hit result actor is NOT the same then that means theirs a new active hitresult that needs to be set up
	if (bHitResultActorIsActive && HitResultActorOfLineTrace != nullptr && HitResultActorOfLineTrace != HitResultActorOfLineTraceExtraTime)
	{
		// need to set old active widget to invisible
		AAI_Character* AIActiveActorExtra = Cast<AAI_Character>(HitResultActorOfLineTraceExtraTime);
		AAI_Character* AIActiveActor = Cast<AAI_Character>(HitResultActorOfLineTrace);
		if (AIActiveActorExtra != nullptr)
		{
			AIActiveActorExtra->SetActiveWidgetToHidden();
			//Keeping HitResultActor reference for transition to ClosestActor;
			HitResultActorOfLineTraceExtraTime = HitResultActorOfLineTrace;

			//setting new active actor to visible
			if (AIActiveActor != nullptr)
			{
				AIActiveActor->SetActiveWidgetToVisible();
				//Setting the new active actor to the selected character bc active = selected
				CurrentlySelectedCharacter = AIActiveActor;
			}
		}
	}
	else if (HitResultActorOfLineTrace != nullptr && !bHitResultActorIsActive)
	{
		AAI_Character* AIActiveActor = Cast<AAI_Character>(HitResultActorOfLineTrace);
		if (AIActiveActor)
		{

			//need to set closest actor to false
			//NEEDS to be before setting HitResult to active because it could be the closest actor itself
			if (ClosestActorAI != nullptr)
			{
				ClosestActorAI->SetActiveWidgetToHidden();
			}

			AIActiveActor->SetActiveWidgetToVisible();
			//Setting selected actor to the new active actor
			CurrentlySelectedCharacter = AIActiveActor;
			//Keeping HitResultActor reference for transition to ClosestActor;
			HitResultActorOfLineTraceExtraTime = HitResultActorOfLineTrace;

			//setting bHitResultActor to active bc active
			bHitResultActorIsActive = true; 
			UE_LOG(LogTemp, Error, TEXT("Is active bool set to true"));

		}
	}
	//if line trace has not hit an actor I want to set ClosestActor widget to visible  // is extra code every call except when trabnsitioning from LineTrace hit to not;
	//need to turn off HitResultActor widget if HitResultActorIs returning nullptr. Can only be done through casting. Using Bool to limit casting amount.
	else if (HitResultActorOfLineTrace == nullptr && bHitResultActorIsActive)
	{
		AAI_Character* HitActorExtraTempForBoolCheck = Cast<AAI_Character>(HitResultActorOfLineTraceExtraTime);
		if (HitActorExtraTempForBoolCheck)
		{
			HitActorExtraTempForBoolCheck->SetActiveWidgetToHidden();
			bHitResultActorIsActive = false;
			UE_LOG(LogTemp, Error, TEXT("Is active bool set to FALSE"));
			// no need for ExtraTime HitResultActor so can set to nullptr altho perhaps unecessary - look into

			HitResultActorOfLineTraceExtraTime = nullptr;
			//if closest actor is set we set visibility on transition, so that if closestactor stays ClosestActor something gets run to turn on visibility.
			if (ClosestActor != nullptr)
			{
				AAI_Character* ClosestActorAI2 = Cast<AAI_Character>(ClosestActor);
				{
					if (ClosestActorAI2 != nullptr)
					{
						ClosestActorAI2->SetActiveWidgetToVisible();
						//setting active actor to SelectedCharacter

					//	UE_LOG(LogTemp, Error, TEXT("Widget Set to active ---"));

						CurrentlySelectedCharacter = ClosestActorAI2;
					}
				}

			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("AI Character at active to closest actor transition returning nullptr should never happen"));
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("HitActorExtraComing as nullptr SHOULD NEVER HAPPEN look into"));
		}
	}


	for (AActor* Actor : CollisionFieldActors /*array*/)
	{
		DistanceToCurrentArrayActor = this->GetDistanceTo(Actor);
		
		//If their is no Closest Actor, we need to set it and set its widget to visible;
		if (ClosestActor == nullptr)
		{
			// if the closest actor is ever a nullptr, then DistanceToClosestActor needs to be reset;
			ClosestActor = Actor;
			ClosestActorAI = Cast<AAI_Character>(ClosestActor);
			if (ClosestActorAI != nullptr)
			{
					ClosestActorAI->SetActiveWidgetToVisible();
					//setting active actor to SelectedCharacter
				//	UE_LOG(LogTemp, Error, TEXT("Widget Set to active ---"));

					CurrentlySelectedCharacter = ClosestActorAI;

				//	UE_LOG(LogTemp, Error, TEXT("SelectedCharacter Set to active ---"));
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Closest Actor AI Cast RETURNING NULLPTR should happen. Find out why"));
			}

		}

		//Need to make sure Current Actors location is constantly set, so it doesn't just keep getting smaller. 
		DistanceToClosestActor = this->GetDistanceTo(ClosestActor);

		// If their is ALREADY A CLOSEST ACTOR than we compare it to check if a new one needs to be set
		if (DistanceToCurrentArrayActor < DistanceToClosestActor && ClosestActor->GetUniqueID() != Actor->GetUniqueID())
		{
			DistanceToClosestActor = DistanceToCurrentArrayActor;

			// We KNOW the closest actor is changing and it is NOT THE SAME the OLD closestActor so we want to HIDE THE OLD CLOSEST ACTOR WIDGET
			if (ClosestActorAI != nullptr)
			{
				ClosestActorAI->SetActiveWidgetToHidden();
			}
			ClosestActor = Actor;
			ClosestActorAI = Cast<AAI_Character>(ClosestActor);

			if (ClosestActorAI != nullptr)
			{
				if (HitResultActorOfLineTrace == nullptr)
				{
					ClosestActorAI->SetActiveWidgetToVisible();
					//setting active actor to SelectedCharacter

				//	UE_LOG(LogTemp, Error, TEXT("Widget Set to active ---"));

					CurrentlySelectedCharacter = ClosestActorAI;

				//	UE_LOG(LogTemp, Error, TEXT("SelectedCharacter Set to active ---"));
				}			
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Closest Actor AI Cast RETURNING NULLPTR should happen. Find out why"));
			}

			UE_LOG(LogTemp, Warning, TEXT("Closest Actor Set For Player -------------------------------------------------------"));
		} 

	}
	//UE_LOG(LogTemp, Warning, TEXT("Checking Timer start rate after hitting play;"));
	  
	//might be useless should check out- ---- Doesn't set widget visibility to 0 bc no actor to act on
	if (CollisionFieldActors.Num() == 0)
	{


		ClosestActor = nullptr;


		UE_LOG(LogTemp, Warning, TEXT("Closest Actor Set to nullptr bc player collision array is empty"));
	}    
}

void APlayerCharacter::ToggleRun()
{
	if (bIsRunning)
	{
		this->GetCharacterMovement()->MaxWalkSpeed = 150;
		bIsRunning = false;
		UE_LOG(LogTemp, Error, TEXT("Walking Toggled On"));
	}
	else if (!bIsRunning) 
	{
		this->GetCharacterMovement()->MaxWalkSpeed = 600;
		bIsRunning = true;
		UE_LOG(LogTemp, Error, TEXT("Running Toggled On"));
	}
}

 
bool APlayerCharacter::bIsAttackAnimMontagePlaying()
{
	if (CharacterAnimInstance && AttackRightMontage)
	{
		if (CharacterAnimInstance->Montage_IsPlaying(AttackRightMontage))
		{
			//UE_LOG(LogTemp, Warning, TEXT("Montage Is Playing Returning true"));
			return true;
		}
		else
		{
			//	UE_LOG(LogTemp, Warning, TEXT("Montage Is NOT Playing Returning false"));
			return false;
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("AICharacterAnimInstance returning FALSE! This shouldn't be happening!"))
			return true;
	}

}

void APlayerCharacter::EngageSelection()
{
	UE_LOG(LogTemp, Error, TEXT("Simple move to actor should be being called -selection engaged-"));

	if (CurrentlySelectedCharacter != nullptr && PlayerController != nullptr)
	{


		if (CurrentlySelectedCharacter->ActorHasTag("FriendlyNPC"))
		{
			//resetting value on player engage so its not autoing the first person who attacked 

			ManuallySelectedEnemyCharacter = CurrentlySelectedCharacter;
			bPlayerInactiveAndEnemyHasAttacked = false;
			bPlayerIsFighting = false;
			//remove AIEnemyForDistanceCheck so that it doesn't get called when the activat ed selection isnt an enemy
			AIEnemyForAttackDistanceCheck = nullptr;

			UAIBlueprintHelperLibrary::SimpleMoveToActor(PlayerController, CurrentlySelectedCharacter);

			
		}
		else if (CurrentlySelectedCharacter->ActorHasTag("EnemyNPC"))
		{
			// Engaging enemy character
			if (CurrentlySelectedCharacter)
			{
				bPlayerInactiveAndEnemyHasAttacked = false;

				//Here gives character for AttackEnemyAuto which will enable / set AIEnemyForDistance check down the line. Have functionality for it being set without engaging in the !bIsInstigatorCheck started by AI adding has attacked actor
				ManuallySelectedEnemyCharacter = CurrentlySelectedCharacter;

				if (!ManuallySelectedEnemyCharacter->bAIIsFightingC)
				{
					//Set turn whose turn is it for both AI and Player
					CurrentlySelectedCharacter->bAIsTurnToAttackC = false;
					CurrentlySelectedCharacter->bEnemysTurnToAttackC = true;

					bPlayersTurnToAttack = true;
					bEnemysTurnToAttack = false;

					//Set Turn Start time for both AI and Player
					DeltaTimePlayerTurnStart = GetWorld()->GetTimeSeconds();
					CurrentlySelectedCharacter->DeltaTimeEnemyTurnStartC = DeltaTimePlayerTurnStart;

					//Set playerAs closestActor for the AI so the AI can target player OR see if adding a sound component can add actors to the percieved actors array

					//Set FightingStart for both AI and Player
					CurrentlySelectedCharacter->bAIIsFightingC = true;
					bPlayerIsFighting = true;

					UAIBlueprintHelperLibrary::SimpleMoveToActor(PlayerController, CurrentlySelectedCharacter);
				} 
				else if (ManuallySelectedEnemyCharacter->bAIIsFightingC)
				{
					if (ManuallySelectedEnemyCharacter->bAIsTurnToAttackC)
					{
						//setting enemy turn
						DeltaTimeEnemyTurnStart = ManuallySelectedEnemyCharacter->DeltaTimeAITurnStartC;
						bEnemysTurnToAttack = true;


						//setting player turn to false
						bPlayersTurnToAttack = false;

						//setting player to fighting
						bPlayerIsFighting = true;
					} 
					else if (ManuallySelectedEnemyCharacter->bEnemysTurnToAttackC)
					{
						//setting enemy turn
						DeltaTimePlayerTurnStart = ManuallySelectedEnemyCharacter->DeltaTimeEnemyTurnStartC;
						bPlayersTurnToAttack = true;

						//setting player turn to false
						bEnemysTurnToAttack = false;

						//setting player to fighting
						bPlayerIsFighting = true;
					}
				}
				StopPlayerMovementAndRotationControl(ManuallySelectedEnemyCharacter);
			}
	
		}



		UE_LOG(LogTemp, Error, TEXT("Simple move to actor is being called"));
	}




	// for debugging
	if (!PlayerController)
	{
		UE_LOG(LogTemp, Error, TEXT("Player controller not found should not be happening"));
	}

	if (!CurrentlySelectedCharacter)
	{
		UE_LOG(LogTemp, Error, TEXT("SelectedCharacter not found should not be happening"));
	}

}

void APlayerCharacter::AddHaveAttackedAICharacterToPlayer(AAI_Character* AttackingAI)
{
	//AI_Character adds has attacked actor, also sets bPlayerIsInstigator to false which sets bPlayerInactiveAndEnemyHasAttacked = true which then triggers this character to be used in AttackEnemyAuto
		PlayerInactiveFirstAttackedEnemy = AttackingAI; 
		bPlayerInactiveAndEnemyHasAttacked = true;
}

void APlayerCharacter::AttackEnemyAuto(AAI_Character* EnemyActor)
{
	// the player attack function starts on engage. So i need a move to + distance check for stop distance then attack
	//attack only works if a character is selected
		if (!bIsMontageActive)
		{

			if (PlayerController)
			{
				UAIBlueprintHelperLibrary::SimpleMoveToActor(PlayerController, EnemyActor);

				float HealthValueForLogging;

				HealthValueForLogging = EnemyActor->AICharacterStatSheet->Health - this->PlayerCharacterSheet->Damage;

				if (HealthValueForLogging <= 0)
				{
					ClosestActor = nullptr;
					bPlayerIsFighting = false;
					StartPlayerMovementAndRotationControl();
				}

				if (HealthValueForLogging <= 0 && EnemyActor->bAIIsDead == false)
				{
					EnemyActor->bAIIsDead = true;
					if (GEngine)
					{
						GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Yellow, FString::Printf(TEXT("Enemy of Player is dead")));
					}

					//	AIEnemyActor->PlayAIDeathAnimation();

					EnemyActor->DetachFromControllerPendingDestroy();
					EnemyActor->SetActorEnableCollision(ECollisionEnabled::NoCollision);

					//Change EnemyActor from using animation instance to a singular animation asset for death animation using Uskeletal Mesh Component
					EnemyActor->AICharacterMesh->SetAnimationMode(EAnimationMode::AnimationSingleNode);
					if (EnemyActor->DeathAnimationMontage)
					{
						EnemyActor->AICharacterMesh->PlayAnimation(EnemyActor->DeathAnimationMontage, false);
						float DeathAnimationLength = EnemyActor->DeathAnimationMontage->GetPlayLength();

						EnemyActor->bAIIsDead = true;
						GetWorldTimerManager().SetTimer(EnemyActor->DeathTimerHandle, EnemyActor, &AAI_Character::AIDeathTimerFunction, DeathAnimationLength - 0.4f, false);
					}
					
				}

				else if (GEngine)
				{
					GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Yellow, FString::Printf(TEXT("Enemy of player Health = %f"), HealthValueForLogging));

					//Setting Enemy Health
					EnemyActor->AICharacterStatSheet->Health = HealthValueForLogging;
				}


				//Enemy Actor needs to be set temporarily so that the distance check can use it
				//used by check in tick dont delete --- USED TO ENABLE TICK THAT STARTS THE ATTACK
				AIEnemyForAttackDistanceCheck = EnemyActor;

				// this need to be checked repeatedly so needs to be on tick for precise stopping for melee animations so that they work properly
				// this needs to be moved to a new function for calling on distance reached

			}

		}
}
 

void APlayerCharacter::PlayerPlayAttackAnimation()
{
	if (AttackRightMontage)
	{
		//make sure its still the players turn
		if (bPlayersTurnToAttack)
		{
			PlayAnimMontage(AttackRightMontage);
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("NO anim for ATTACKing shouldnt HAPPEn"));
	}
}

void APlayerCharacter::FreeCamera()
{
	if (CameraIsFree == true)
	{
		CameraIsFree = false;
		UE_LOG(LogTemp, Warning, TEXT("Camera is not longer free"));
		
		
	}
	else if (CameraIsFree == false)
	{
		CameraIsFree = true;
		UE_LOG(LogTemp, Warning, TEXT("Camera is free"));
	}
}

void APlayerCharacter::StopPlayerMovementAndRotationControl(AAI_Character* EngagedActor)
{
	//Disable Use Controller Desired Rotation so that the player cant effect rotation while fighting
	this->GetCharacterMovement()->bUseControllerDesiredRotation = false;

	//Stop Player from moving the Character
	bPlayerCanMove = false;

	//Rotate the player to look at the engaged enemy
	if (EngagedActor)
	{
		FVector AILocation = EngagedActor->GetActorLocation();

		FVector PlayerLocation = this->GetActorLocation();

		FRotator PlayerFaceLocation = UKismetMathLibrary::FindLookAtRotation(PlayerLocation, AILocation);

		float PlayerFaceLocationYaw = PlayerFaceLocation.Yaw;

		FRotator PlayerFaceLocationJustYaw = FRotator(0, PlayerFaceLocationYaw, 0);



		this->SetActorRotation(PlayerFaceLocationJustYaw);

		UE_LOG(LogTemp, Warning, TEXT("PLAYER SHOULD BE ROTATING -------------------------------"));
	}

}

void APlayerCharacter::StartPlayerMovementAndRotationControl()
{
	//Disable Use Controller Desired Rotation so that the player cant effect rotation while fighting
	this->GetCharacterMovement()->bUseControllerDesiredRotation = true;

	//Stop Player from moving the Character
	bPlayerCanMove = true;

	//Rotate the player to look at the engaged enemy
}

void APlayerCharacter::PlayerDeathTimerFunction()
{
	PauseGameAndOpenMenu();
	SHMouse();
	TurnOff();
	SetActorTickEnabled(false);
	GetWorldTimerManager().ClearTimer(TimerHandle);
	

}

void APlayerCharacter::PauseGameAndOpenMenu()
{
	if (MenuWidget)
	{
		UE_LOG(LogTemp, Display, TEXT("MENU WIDGET RETURNING TRUE SHOULD BE BEING SPAWNED ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"));

		MenuWidget->AddToViewport();
		UGameplayStatics::SetGamePaused(GetWorld(), true);
	}

}

void APlayerCharacter::QuitGameTimerFunction()
{
	UKismetSystemLibrary::QuitGame(GetWorld(), GetWorld()->GetFirstPlayerController(), EQuitPreference::Quit, false);
}

void APlayerCharacter::SaveGame()
{
	//Creat an instance of our saved game class
	USaveGameM* SaveGameInstance = Cast<USaveGameM>(UGameplayStatics::CreateSaveGameObject(USaveGameM::StaticClass()));
	HeartsGameMode = Cast<AHeartsGameModeBase>(GetWorld()->GetAuthGameMode());

	//before saving the player save slots I need to add this new slot to the array

	bool bIsSaveNameUnique = true;
	for (FText SlotName : HeartsGameMode->GameModeBaseSavedGameSlotNameArray)
	{
		if (SlotName.ToString() == SaveGameSlotName.ToString())
		{
			bIsSaveNameUnique = false;
		}
	}
	if (bIsSaveNameUnique)
	{
		HeartsGameMode->GameModeBaseSavedGameSlotNameArray.Add(SaveGameSlotName);
	}



	//saving PlayerSaveSlotNames into save slot for access
	SavePlayerSavesSlotNames();
	

	//Transfer variables from current game to save game instance
	//for player
	SaveGameInstance->PlayerLocation = this->GetActorLocation();
	if (this->GetController())
	{
		SaveGameInstance->PlayerControllerRotation = this->GetController()->GetControlRotation();
		SaveGameInstance->PlayerRotation = this->GetActorRotation();
	}
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf(TEXT("BeforeSaving health = %f"), PlayerCharacterSheet->Health));
	}
	SaveGameInstance->PlayerHealth = this->PlayerCharacterSheet->Health;
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf(TEXT("Player Health = %f saving game"), PlayerCharacterSheet->Health));
	}

	//for AI near player updating
	for (AActor* AIActor : CollisionFieldActors)
	{
		AAI_Character* AICharacterForGettingVariables = CastChecked<AAI_Character>(AIActor);
		
		FInDistanceAICharacterVariableHolder CurrentAICharacterVariables;

		FString CurrentAICharacterName = AICharacterForGettingVariables->GetName();

		CurrentAICharacterVariables.AIName = CurrentAICharacterName;

		CurrentAICharacterVariables.AIUniqueID = AICharacterForGettingVariables->UniqueID;

		CurrentAICharacterVariables.AIHealth = AICharacterForGettingVariables->AICharacterStatSheet->Health;

		CurrentAICharacterVariables.AIControllerRotation = AICharacterForGettingVariables->GetControlRotation();

		CurrentAICharacterVariables.AILocation = AICharacterForGettingVariables->GetActorLocation();
		

		SaveGameInstance->InDistanceAICharacterVariables.Add(CurrentAICharacterName, CurrentAICharacterVariables);
		
	}
	//for all AI
	TArray<AActor*> AllAIForSavingVariables;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AAI_Character::StaticClass(), AllAIForSavingVariables);
	for (AActor* AIActor : AllAIForSavingVariables)
	{
		AAI_Character* AICharacterForGettingVariables = CastChecked<AAI_Character>(AIActor);
		FString CurrentAICharacterName = AICharacterForGettingVariables->GetName();

		if (SaveGameInstance->InDistanceAICharacterVariables.Contains(CurrentAICharacterName))
		{
	
			FInDistanceAICharacterVariableHolder CurrentAICharacterVariables = SaveGameInstance->InDistanceAICharacterVariables[CurrentAICharacterName];;

			CurrentAICharacterVariables.bIsAIDead = AICharacterForGettingVariables->bAIIsDead;

			SaveGameInstance->InDistanceAICharacterVariables.Add(CurrentAICharacterName, CurrentAICharacterVariables);
		}

	}

	//SaveGameInstance->InDistanceAICharactersArray = this->CollisionFieldActors;


	//Save the game
	UGameplayStatics::SaveGameToSlot(SaveGameInstance, SaveGameSlotName.ToString(), 0);

	


	//Show we have saved game
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf(TEXT("GAME WAS SAVED UNDER NAME: %s"), *SaveGameSlotName.ToString()));
	}

}

void APlayerCharacter::SetSaveGameNameVariabled(FText SaveGameName)
{
	SaveGameSlotName = SaveGameName;
}

void APlayerCharacter::LoadGame()
{

	//Creat an instance of our saved game class

	//Load the saved Game into saved game instance variable
	//stops LoadGameSlot from crashing game when not set. This function gets called on beginplay on the level blueprint. If no save file then crash without this if
	if (LoadGameSlotName.IsEmpty())
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, FString::Printf(TEXT("LOAD GAME SLOT NAME IS EMPTY, SHOULD NEVER HAPPEN")));
		}
	}
	else
	{
		USaveGameM* SaveGameInstance = Cast<USaveGameM>(UGameplayStatics::CreateSaveGameObject(USaveGameM::StaticClass()));
		SaveGameInstance = Cast<USaveGameM>(UGameplayStatics::LoadGameFromSlot(LoadGameSlotName.ToString(), 0));
		HeartsGameMode = Cast<AHeartsGameModeBase>(GetWorld()->GetAuthGameMode());

		//if this is a new game, no variables need to be saved -- but there still needs to be a new game slot because level is loaded based on the LoadGameSlotName at every construction
		if (LoadGameSlotName.ToString() != NewGameSlotNameWithJargon.ToString())
		{
			//transfer variables from saved game back to game
		//for player
		//player location isnt set from New Game file. So to prevent player from spawning in 0,0,0 location I need this check -- wait do I tho? I really don't think I do. Just double check b4 delete
			if (SaveGameInstance->PlayerLocation != FVector(0, 0, 0))
			{
				this->SetActorLocation(SaveGameInstance->PlayerLocation);
				this->GetController()->SetControlRotation(SaveGameInstance->PlayerControllerRotation);
				this->SetActorRotation(SaveGameInstance->PlayerRotation);

				if (GEngine)
				{
					GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, FString::Printf(TEXT("1Player Health = %f"), PlayerCharacterSheet->Health));
				}
				this->PlayerCharacterSheet->Health = SaveGameInstance->PlayerHealth;
				if (GEngine)
				{
					GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, FString::Printf(TEXT("2Player Health = %f"), PlayerCharacterSheet->Health));
				}
			}

		//	GetWorldTimerManager().SetTimer(SetAIVariablesTimer, this, &APlayerCharacter::SetAIVariables, 3.0f, false);
		//	SetAIVariables();
		}

		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, FString::Printf(TEXT("3Player Health = %f"), PlayerCharacterSheet->Health));
		}
		//Gets and sets PlayerSavesSlotNames array for save/Load game
		GetPlayerSavesSlotNames();

		//show game was loaded
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, FString::Printf(TEXT("GAME WAS LOADED")));
		}
	}

}

void APlayerCharacter::SaveSelectedLoadGameName(FText SelectedLoadGameSlotName)
{
	USaveGameM* SaveGameInstance = Cast<USaveGameM>(UGameplayStatics::CreateSaveGameObject(USaveGameM::StaticClass()));


	//Save the load file name
	SaveGameInstance->SelectedLoadGameSlotName = SelectedLoadGameSlotName;

	//Save the Instance
	UGameplayStatics::SaveGameToSlot(SaveGameInstance, SelectedLoadSlotNameWithJargon.ToString(), 0);
}

void APlayerCharacter::LoadSelectedLoadGameName()
{
		//load the save that is specifically only holding the selected load game slot name
		USaveGameM* SaveGameInstance = Cast<USaveGameM>(UGameplayStatics::CreateSaveGameObject(USaveGameM::StaticClass()));
		SaveGameInstance = Cast<USaveGameM>(UGameplayStatics::LoadGameFromSlot(SelectedLoadSlotNameWithJargon.ToString(), 0));

		if (SaveGameInstance)
		{
			//set that slot name to be loaded when LoadGame is called on level blueprint isConstructed
			this->LoadGameSlotName = SaveGameInstance->SelectedLoadGameSlotName;
		}

		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Orange, FString::Printf(TEXT("LOAD THE SELECTED GAME MODE NAME IS BEING CALLED ----")));
		}
	

}

//this needs to be played before open level gets called after NewGame is pressed by user so that the SlotNameArray is always saved
void APlayerCharacter::SaveNewGameSlot()
{
	USaveGameM* SaveGameInstance = Cast<USaveGameM>(UGameplayStatics::CreateSaveGameObject(USaveGameM::StaticClass()));

	//Save the Instance
	UGameplayStatics::SaveGameToSlot(SaveGameInstance, NewGameSlotNameWithJargon.ToString(), 0);
}

void APlayerCharacter::SavePlayerSavesSlotNames()
{
	USaveGameM* SaveGameInstance = Cast<USaveGameM>(UGameplayStatics::CreateSaveGameObject(USaveGameM::StaticClass()));

	//for game mode
	if (HeartsGameMode)
	{
		//saving save file slot names
		SaveGameInstance->SaveFileSavedGameSlotNameArray = HeartsGameMode->GameModeBaseSavedGameSlotNameArray;
		float SlotNameArrayLength = HeartsGameMode->GameModeBaseSavedGameSlotNameArray.Num();

		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, FString::Printf(TEXT("Number of Saves in save array = %f SAVED being saved"), SlotNameArrayLength));
		}
	}

	else

	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, FString::Printf(TEXT("HEARTS GAME MODE NOT BEING LOADED")));
		}
	}

	//Save the Instance
	UGameplayStatics::SaveGameToSlot(SaveGameInstance, PlayerSlotNamesArrayWithJargon.ToString(), 0);
}

void APlayerCharacter::GetPlayerSavesSlotNames()
{
	//load the save that is specifically only holding the selected load game slot name
	USaveGameM* SaveGameInstance = Cast<USaveGameM>(UGameplayStatics::CreateSaveGameObject(USaveGameM::StaticClass()));
	SaveGameInstance = Cast<USaveGameM>(UGameplayStatics::LoadGameFromSlot(PlayerSlotNamesArrayWithJargon.ToString(), 0));

	if (SaveGameInstance)
	{
		if (HeartsGameMode)
		{
			//adding the save slot names to the game mode base for reference
			HeartsGameMode->GameModeBaseSavedGameSlotNameArray = SaveGameInstance->SaveFileSavedGameSlotNameArray;
			float SlotNameArrayLength = HeartsGameMode->GameModeBaseSavedGameSlotNameArray.Num();

			//Show we have saved game
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, FString::Printf(TEXT("Number of Saves in save array = %f loaded"), SlotNameArrayLength));
			}
		}
	}


}

void APlayerCharacter::DeleteSaveGameFile(FText SaveGameSlotNameForDeletion)
{
	UGameplayStatics::DeleteGameInSlot(SaveGameSlotNameForDeletion.ToString(), 0);
}



