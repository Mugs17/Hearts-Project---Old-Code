 // Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Animation/BlendSpace.h"
#include "CharacterStatisticsComponent.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "ViewPointLine.h"
#include "HeartsGameModeBase.h"
#include "SaveGameM.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "Components/WidgetComponent.h"
#include "CoreMinimal.h"
#include "AI_Character.h"  
#include "Components/SphereComponent.h"
#include "PlayerCharacter.generated.h"

class AWeapon;

UCLASS()
class HEARTS_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayerCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


	UPROPERTY(VisibleAnywhere)      
	UViewPointLine* ActiveTraceLineComponent;

	UActorComponent* ActorComponentTest;

	AActor* HitResultActorOfLineTrace;

	bool bHitResultActorIsActive = false;

	AActor* HitResultActorOfLineTraceExtraTime;

	//UPROPERTY(VisibleAnywhere)
	//	class UWidgetComponent* ActiveWidgetComponent;



public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

//UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Anims")
//		UBlendSpace* BlendSpace;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Anim)
		UAnimMontage* JumpMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Anim)
		UAnimMontage* AttackRightMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Anim)
		UAnimMontage* DeathAnimMontage;

	void PlayPlayerDeathAnimation();


	USphereComponent* MyCollisionSphere;
	

private:


	void MoveForward(float AxisValue);
	void MoveRight(float AxisValue);
	void LookUp(float AxisValue);
	void LookRight(float AxisValue);

	void QuitGame();

	void Jump();

	void AttackRight();


	// For Collision Sphere

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);


	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<AWeapon> WeaponClass;

	UPROPERTY()
		AWeapon* Weapon;

	UPROPERTY()
		TArray<AActor*> CollisionFieldActors;


	float DistanceToCurrentArrayActor = 200000;

	float DistanceToClosestActor = 20000;

	AActor* ClosestActor;

	void MyTimerFunction();

	FTimerHandle TimerHandle;

	bool bIsRunning = true;

	void ToggleRun();

	//for AI widget



	AAI_Character* ClosestActorAI;

	public:


	// for fighting AI

	bool bPlayerIsFighting = false;

	bool bPlayersTurnToAttack = false;

	bool bEnemysTurnToAttack = false;

	bool bPlayerHasAttacked = false;


	float DeltaTimeFightStart;

	float DeltaTimeFightEnd;

	float DeltaTimePlayerTurnStart;

	float DeltaTimePlayerTurnEnd;

	float DeltaTimeEnemyTurnStart;

	float DeltaTimeEnemyTurnEnd;

	float FightRoundTime = 1.5;

	void AttackEnemyAuto(AAI_Character* EnemyActor);


	//montage 

	bool bIsMontageActive = false;

	bool bIsAttackAnimMontagePlaying();

	UAnimInstance* CharacterAnimInstance;

	USkeletalMeshComponent* CharacterMesh;

	// Selection

	void EngageSelection();

	AAI_Character* CurrentlySelectedCharacter;

	//controller
	AController* PlayerController;

	//Getting attacking actor information so that i auto attack back if the attacking actor isnt the closest and also isnt selected aka the active actor;

	AAI_Character* ClosestHaveAttackedEnemyAI;

	float ClosestAttackedEnemyAIDistance;

	//need bool for case where i get attacked but attacked isnt closest actor and im not attacking anyone, so i go fight that person automatically. Should be false for all other cases
	
	bool bPlayerInactiveAndEnemyHasAttacked = false;

	// need function for enemy ai to call to set variable altho with UPROPERTY() I could prob just straight up set it -- will do in future
	void AddHaveAttackedAICharacterToPlayer(AAI_Character* AttackingAI);

	AAI_Character* PlayerInactiveFirstAttackedEnemy;

	AAI_Character* ManuallySelectedEnemyCharacter;

	void PlayerPlayAttackAnimation();

	AAI_Character* AIEnemyForAttackDistanceCheck;

	void CancelCombat();

	//For Camera

	void FreeCamera();

	bool CameraIsFree = false;

	//For character fight start movement / camera control

	void StopPlayerMovementAndRotationControl(AAI_Character* EngagedActor = nullptr);

	void StartPlayerMovementAndRotationControl();

	bool bPlayerCanMove = true;

	// for statistics

	UCharacterStatisticsComponent* PlayerCharacterSheet; 

	//for death

	bool bPlayerIsDead = false;

	FTimerHandle PlayerDeathTimerHandle;

	void PlayerDeathTimerFunction();

	// for menu widget
	// THIS ISNT BEING SET IN BLUEPRINT NO IDEA WHY
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Widget)
		UUserWidget* MenuWidget;

	void PauseGameAndOpenMenu();
	

	void SHMouse();

	//timer for quitting game after period of time after player death

	FTimerHandle QuitGameTimerHandle;

	void QuitGameTimerFunction();

	//Save and Load Game functions

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FText SaveGameSlotName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FText LoadGameSlotName;

	UFUNCTION(BlueprintCallable)
	void SaveGame();

	UFUNCTION(BlueprintCallable)
	void SetSaveGameNameVariabled(FText SaveGameName);

	UFUNCTION(BlueprintCallable)
	void LoadGame();

	//Saving the string for the selected save to be loaded based on so that it persists and can be used on Begin play of the level
	UFUNCTION(BlueprintCallable)
	void SaveSelectedLoadGameName(FText LoadFileSlotName);

	//loading the selected Load Game Slot name on Level blueprint isConstructed
	UFUNCTION(BlueprintCallable)
	void LoadSelectedLoadGameName();

	//always needs to be set on NewGame
	UFUNCTION(BlueprintCallable)
	void SaveNewGameSlot();

	//saving all save string names in specific slot for access
	UFUNCTION(BlueprintCallable)
		void SavePlayerSavesSlotNames();

	//getting array of save string names in specific slot for access
	UFUNCTION(BlueprintCallable)
		void GetPlayerSavesSlotNames();

	//Created unduplicatable string so that the SelectedLoadSlotName is unreplaceable
	FText SelectedLoadSlotNameWithJargon = FText::FromString(FString(TEXT("qwierha12asdfasdfa f4ef323okokokokksdfsfe34223edawfdscbggggg54")));

	//Created unduplicatable string so that the NewGame Slot Name is unreplaceable
	UPROPERTY(EditAnywhere, BlueprintReadWrite) //so that can be called from blueprint on level constructed
	FText NewGameSlotNameWithJargon = FText::FromString(FString(TEXT("okayoa23 sdafsdfasdfasdfdsfe4e42323dsfgbhhjhjjkk s")));

	//Created unduplicatable string so that the SelectedLoadSlotName is unreplaceable
	FText PlayerSlotNamesArrayWithJargon = FText::FromString(FString(TEXT("asdfadf hhhdy dydywemnwhoiatofnedor3smfnas38i")));

	//deleting actual save game slot
	UFUNCTION(BlueprintCallable)
	void DeleteSaveGameFile(FText SaveGameSlotNameForDeletion);


//	FText NewGameSlotName


	//Game Mode Reference

	AHeartsGameModeBase* HeartsGameMode;


	//location for player start at begging of game so player can be reset to this position

	//For Getting AI Variable on load

	FTimerHandle SetAIVariablesTimer;




};
