// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuWidget.h"

void UMenuWidget::SpawnImageUI()
{
	UE_LOG(LogTemp, Warning, TEXT("Image UI SPAWNED"));

}

void UMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SpawnImageUI();
}