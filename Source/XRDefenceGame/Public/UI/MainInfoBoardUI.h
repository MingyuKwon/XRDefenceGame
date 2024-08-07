// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "XRDefenceEnums.h"
#include "MainInfoBoardUI.generated.h"

/**
 * 
 */
class UTextBlock;
class UCanvasPanel;

UCLASS()
class XRDEFENCEGAME_API UMainInfoBoardUI : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* TimeText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* BlueHeartText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* PurpleHeartText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* OrangeHeartText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* HealthText;

	UPROPERTY(meta = (BindWidget))
	UCanvasPanel* GameStartPanel;

	UPROPERTY(meta = (BindWidget))
	UCanvasPanel* GameEndtPanel;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* GameStart_GameStateText;


public:
	UFUNCTION(BlueprintCallable)
	void SetTimeText(float LeftSecond);

	UFUNCTION(BlueprintCallable)
	void SetBlueHeartText(float HealthAmount);

	UFUNCTION(BlueprintCallable)
	void SetPurpleHeartText(float HealthAmount);

	UFUNCTION(BlueprintCallable)
	void SetOrangeHeartText(float HealthAmount);

	UFUNCTION(BlueprintCallable)
	void SetHealthText(float HealthAmount);


	UFUNCTION(BlueprintCallable)
	void WhichPanelToShow(EGameMatchState matchState);

	UFUNCTION(BlueprintCallable)
	void SetGameStateText(EGameMatchState matchState, FString text);


};
