// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GamePlayHandUI.generated.h"

/**
 * 
 */
class UTextBlock;

UCLASS()
class XRDEFENCEGAME_API UGamePlayHandUI : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* TimeText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* GoldText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* BlueHeartText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* PurpleHeartText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* OrangeHeartText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* HealthText;


public:
	UFUNCTION(BlueprintCallable)
	void SetTimeText(float LeftSecond);

	UFUNCTION(BlueprintCallable)
	void SetGoldText(float GoldAmount);

	UFUNCTION(BlueprintCallable)
	void SetBlueHeartText(float HealthAmount);

	UFUNCTION(BlueprintCallable)
	void SetPurpleHeartText(float HealthAmount);

	UFUNCTION(BlueprintCallable)
	void SetOrangeHeartText(float HealthAmount);

	UFUNCTION(BlueprintCallable)
	void SetHealthText(float HealthAmount);




};
