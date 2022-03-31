// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "ActiveActorWidget.generated.h"

/**
 * 
 */
UCLASS()
class HEARTS_API UActiveActorWidget : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY(meta = (BindWidget))
	UImage* ImageIdentifierWidget;


	void SpawnImageUI();

protected:

	void NativeConstruct() override;


};
	

