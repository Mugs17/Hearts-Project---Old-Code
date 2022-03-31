// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "CharacterStatisticsComponent.h"
#include "GameFramework/Pawn.h"
#include "Animation/BlendSpace.h"
#include "Animation/AnimInstance.h"
#include "Engine/SkeletalMesh.h"
#include "GameFramework/Character.h"
#include "Components/WidgetComponent.h"
#include "ActiveActorWidget.h"
#include "AI_Character.generated.h"

class AWeapon;

UCLASS()
class HEARTS_API AAI_Character : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AAI_Character();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ID)
		float UniqueID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class AWaypoint* NextWaypoint;

	UPROPERTY()
	UActiveActorWidget* ActiveWidgetForActor;

	void SetActiveWidgetToHidden();

	void SetActiveWidgetToVisible();

	void AIToggleRun(bool bIsRunning);

	// For attacking
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Anim)
		UAnimMontage* SwordAttackAnimation;
	
	//death animation
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Anim)
		UAnimMontage* DeathAnimationMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Anim)
		UAnimationAsset* DeathAnimationAsset;

	void PlayAIDeathAnimation();

	bool IsAttackAnimMontagePlaying();

	bool bIsMontagePlaying(UAnimMontage* AnimMontageToCheck);

	UAnimInstance* AICharacterAnimInstance;

	USkeletalMeshComponent* AICharacterMesh;

	//bool bIsMontageStopped(UAnim)


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


private:
	UPROPERTY(VisibleAnywhere)
		USceneComponent* AIRootComponent;

	UPROPERTY(VisibleAnywhere)
		class UWidgetComponent* ActiveWidgetComponent;

	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<AWeapon> WeaponClass;
public:
	UPROPERTY()
		AWeapon* AIWeapon;


public:

	UPROPERTY()
	bool bAIIsFightingC = false;

	UPROPERTY()
	bool bAIsTurnToAttackC = false;

	UPROPERTY()
	bool bEnemysTurnToAttackC = false;

	UPROPERTY()
	bool bAIHasAttackedC = false;

	UPROPERTY()
	bool bIsAIInstigatorC = false;

	////////

	float DeltaTimeFightStartC;

	float DeltaTimeFightEndC;

	float DeltaTimeAITurnStartC;

	float DeltaTimeAITurnEndC;

	UPROPERTY()
		float DeltaTimeEnemyTurnStartC;

	float DeltaTimeEnemyTurnEndC;

	float FightRoundTimeC = 1.5;

	bool bIsHeard;


	// For Collision Sphere
	

	UFUNCTION()
		void OnSphereOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		void OnSphereOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY()
		TArray<AActor*> AISphereCollisionFieldActorsC;

	USphereComponent* MyAICollisionSphere;

	//For CollisionBox

	UFUNCTION()
		void OnBoxOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		void OnBoxOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY()
		TArray<AActor*> AIBoxCollisionFieldActorsC;

	UBoxComponent* MyAICollisionBox;

	//for statistic

	UCharacterStatisticsComponent* AICharacterStatSheet;

	//for death

	bool bAIIsDead = false;

	bool bAIIsDeadAndMontageIsFinished = false;

	void AIDeathTimerFunction();

	FTimerHandle DeathTimerHandle;




};
