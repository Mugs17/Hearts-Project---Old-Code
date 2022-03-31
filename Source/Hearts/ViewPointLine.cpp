// Fill out your copyright notice in the Description page of Project Settings.


#include "ViewPointLine.h"
#include "DrawDebugHelpers.h"

// Sets default values for this component's properties
UViewPointLine::UViewPointLine()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UViewPointLine::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UViewPointLine::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	
	FVector OUT_PlayerViewPointLocation;

	FRotator OUT_PlayerViewPointRotation;

	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(OUT_PlayerViewPointLocation, OUT_PlayerViewPointRotation);


	FVector PlayerViewPointLineEnd = OUT_PlayerViewPointLocation + OUT_PlayerViewPointRotation.Vector() * DebugLineReach;

	//DrawDebugLine(GetWorld(), OUT_PlayerViewPointLocation, PlayerViewPointLineEnd, FColor::Blue, false, 0.0f, 0.0f, 5);

	FHitResult Hit;

	FCollisionQueryParams TraceParams(FName(""), false, GetOwner());


	GetWorld()->LineTraceSingleByChannel(Hit, OUT_PlayerViewPointLocation, PlayerViewPointLineEnd, ECollisionChannel::ECC_GameTraceChannel1, TraceParams);

	//Hit Actor when touched by line trace is set YAY
	HitActor = Hit.GetActor();

	if (HitActor)
	{
		//UE_LOG(LogTemp, Error, TEXT("This Actor type got hit by LineTrace: %s"), *(HitActor->GetName()));
		GetOwner();
	}
	// HitActor needs to be unset when LineTrace comes back empty - to revert to closest distance ActiveWidget
	else
	{
		HitActor = nullptr;
	}
}

AActor* UViewPointLine::ReturnActorInLineTrace()
{
	return HitActor;
}

