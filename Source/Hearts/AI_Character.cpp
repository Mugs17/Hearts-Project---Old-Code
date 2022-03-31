// Fill out your copyright notice in the Description page of Project Settings.


#include "AI_Character.h" 
#include "GameFramework/CharacterMovementComponent.h"
#include "Weapon.h"

// Sets default values
AAI_Character::AAI_Character()
{

	ActiveWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("ActiveImageWidget"));
	ActiveWidgetComponent->SetupAttachment(RootComponent);

	AICharacterStatSheet = CreateDefaultSubobject<UCharacterStatisticsComponent>(TEXT("Character Statistics Component"));
//
	//RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	//SetRootComponent(RootComponent);
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->bUseControllerDesiredRotation = true;


	GetCharacterMovement()->RotationRate = FRotator(0.0f, 50.0f, 0.0f);
	GetCharacterMovement()->MaxWalkSpeed = 150;
 

	//For Active AI widgetb
	
	//ActiveWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("ActiveImageWidget"));
   // ActiveWidgetComponent->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("CollisionSphereSocket"));


	//AIRootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("AI Root"));
	//SetRootComponent(AIRootComponent);

	//ActiveWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("ActiveWidgetComponent"));
	//ActiveWidgetComponent->SetupAttachment(AIRootComponent);

	
	AISphereCollisionFieldActorsC.Reserve(600);
	

}

void AAI_Character::SetActiveWidgetToHidden()
{
	if (ActiveWidgetForActor != nullptr)
	{
		ActiveWidgetForActor->SetVisibility(ESlateVisibility::Collapsed);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ActiveWidgetForActor returning nullptr ---"));
	}
}

void AAI_Character::SetActiveWidgetToVisible()
{
	if (ActiveWidgetForActor != nullptr)
	{
		ActiveWidgetForActor->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ActiveWidgetForActor returning nullptr ---"));
	}
}

void AAI_Character::AIToggleRun(bool bIsRunning)
{
	if (!bIsRunning)
	{
		this->GetCharacterMovement()->MaxWalkSpeed = 150;
		UE_LOG(LogTemp, Error, TEXT("Walking Toggled On2"));
	
	}
	else if (bIsRunning)
	{
		this->GetCharacterMovement()->MaxWalkSpeed = 600;
		UE_LOG(LogTemp, Error, TEXT("Running Toggled On2"));

	}
}


void AAI_Character::PlayAIDeathAnimation()
{
	if (DeathAnimationMontage)
	{
		PlayAnimMontage(DeathAnimationMontage);
	}

	
}

bool AAI_Character::IsAttackAnimMontagePlaying()
{
	 if (AICharacterAnimInstance)
	{
		if (AICharacterAnimInstance->Montage_IsPlaying(SwordAttackAnimation))
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

bool AAI_Character::bIsMontagePlaying(UAnimMontage* AnimMontageToCheck)
{
	if (AICharacterAnimInstance)
	{
		if (AICharacterAnimInstance->Montage_IsPlaying(AnimMontageToCheck))
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


// Called when the game starts or when spawnedB
void AAI_Character::BeginPlay()
{
	Super::BeginPlay();

	AIWeapon = GetWorld()->SpawnActor<AWeapon>(WeaponClass);
	AIWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("WeaponSocketR"));
	AIWeapon->SetOwner(this);

	ActiveWidgetForActor = Cast<UActiveActorWidget>(ActiveWidgetComponent->GetWidget());

	if (ActiveWidgetForActor != nullptr)
	{
		ActiveWidgetForActor->SetVisibility(ESlateVisibility::Collapsed);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ActiveWidgetForActor returning nullptr ---"));
	}

	AICharacterMesh = GetMesh();


	AICharacterAnimInstance = (AICharacterMesh) ? AICharacterMesh->GetAnimInstance() : nullptr;

	if (!AICharacterAnimInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("AICharacterInstance and Mesh not beign set in constructor"));
	}

	//SettingUp collision sphere
	MyAICollisionSphere = NewObject<USphereComponent>(this);
	MyAICollisionSphere->AttachToComponent(this->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("CollisionSphereSocket"));
	MyAICollisionSphere->RegisterComponent();


	MyAICollisionSphere->InitSphereRadius(5.0f);


	MyAICollisionSphere->ShapeColor = FColor::Yellow;
	MyAICollisionSphere->SetIsReplicated(true);
	MyAICollisionSphere->SetVisibility(true);
	MyAICollisionSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	//MyCollisionSphere->SetCollisionObjectType(ECC_GameTraceChannel2)
	MyAICollisionSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	MyAICollisionSphere->SetCollisionResponseToChannel(ECC_GameTraceChannel3, ECR_Overlap);
	MyAICollisionSphere->SetCollisionResponseToChannel(ECC_GameTraceChannel4, ECR_Overlap);
	//trying to attemp line trace collision
	//MyCollisionSphere->Trace
	MyAICollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &AAI_Character::OnSphereOverlapBegin);
	MyAICollisionSphere->OnComponentEndOverlap.AddDynamic(this, &AAI_Character::OnSphereOverlapEnd);

	MyAICollisionSphere->SetSphereRadius(2000.0f);

	MyAICollisionSphere->SnapTo(this->GetMesh(), TEXT("CollisionSphereSocket"));



	//SettingUp collision sphere
	MyAICollisionBox = NewObject<UBoxComponent>(this);
	MyAICollisionBox->AttachToComponent(this->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("CollisionBoxSocket"));
	MyAICollisionBox->RegisterComponent();

	MyAICollisionBox->InitBoxExtent(FVector(5.0f, 5.0, 5.0));

	MyAICollisionBox->ShapeColor = FColor::Blue;
	MyAICollisionBox->SetIsReplicated(true);
	MyAICollisionBox->SetVisibility(true);
	MyAICollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	//MyCollisionSphere->SetCollisionObjectType(ECC_GameTraceChannel2)
	MyAICollisionBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	MyAICollisionBox->SetCollisionResponseToChannel(ECC_GameTraceChannel3, ECR_Overlap);
	MyAICollisionBox->SetCollisionResponseToChannel(ECC_GameTraceChannel4, ECR_Overlap);
	//trying to attemp line trace collision
	//MyCollisionSphere->Trace
	MyAICollisionBox->OnComponentBeginOverlap.AddDynamic(this, &AAI_Character::OnBoxOverlapBegin);
	MyAICollisionBox->OnComponentEndOverlap.AddDynamic(this, &AAI_Character::OnBoxOverlapEnd);

	MyAICollisionBox->SetBoxExtent(FVector(1000.0f, 2000.0f, 1000.0f));

	MyAICollisionBox->SnapTo(this->GetMesh(), TEXT("CollisionBoxSocket"));

	
	// MyAICollisionBox->LineThickness(50.0f);
}

// Called every frame
void AAI_Character::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AAI_Character::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AAI_Character::OnSphereOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("SUCCESFUL OVERLAP BEGIN----------------------"));

	if (OtherActor != nullptr)
	{
		AISphereCollisionFieldActorsC.Add(OtherActor);
		//	UE_LOG(LogTemp, Warning, TEXT("OtherActor Added"));
	}

}

void AAI_Character::OnSphereOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	UE_LOG(LogTemp, Warning, TEXT("SUCCESFUL OVERLAP END-----------------------"));

	AISphereCollisionFieldActorsC.Remove(OtherActor);
}

void AAI_Character::OnBoxOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor != nullptr)
	{
		AIBoxCollisionFieldActorsC.Add(OtherActor);
	}
	UE_LOG(LogTemp, Warning, TEXT("SUCCESFUL OVERLAP BEGIN++++++++++++++++"));
}

void AAI_Character::OnBoxOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor != nullptr)
	{
		AIBoxCollisionFieldActorsC.Remove(OtherActor);
	}
	UE_LOG(LogTemp, Warning, TEXT("SUCCESFUL OVERLAP END+++++++++++++++"));
}

void AAI_Character::AIDeathTimerFunction()
{

	this->TurnOff();

}
