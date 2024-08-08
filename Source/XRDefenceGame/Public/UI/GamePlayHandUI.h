// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "XRDefenceEnums.h"
#include "GamePlayHandUI.generated.h"

/**
 * 
 */
class UTextBlock;
class UProgressBar;

UCLASS()
class XRDEFENCEGAME_API UGamePlayHandUI : public UUserWidget
{
	GENERATED_BODY()

protected:

	UPROPERTY(meta = (BindWidget))
	UTextBlock* GoldText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* currentGoldMineText;


	UPROPERTY(meta = (BindWidget))
	UProgressBar* CoolTimeProgressBar;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* CoolTimeText;


	UPROPERTY(meta = (BindWidget))
	UTextBlock* OffenceDefenceText;

	

public:
	UFUNCTION(BlueprintCallable)
	void SetGoldText(float GoldAmount, float MaxGoldAmount, float GoldMine);

	UFUNCTION(BlueprintCallable)
	void SetCoolTimeProgressBar(float precent);

	UFUNCTION(BlueprintCallable)
	void SetOffenceDefenceText(EObjectType objectType);



};
