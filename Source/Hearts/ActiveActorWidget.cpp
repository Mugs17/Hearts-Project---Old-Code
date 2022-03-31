// Fill out your copyright notice in the Description page of Project Settings.


#include "ActiveActorWidget.h"

void UActiveActorWidget::SpawnImageUI()
{
	UE_LOG(LogTemp, Warning, TEXT("Image UI SPAWNED"));

}

void UActiveActorWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SpawnImageUI();
}