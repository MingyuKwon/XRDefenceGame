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
class UImage;

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

	// Game Start
	UPROPERTY(meta = (BindWidget))
	UTextBlock* GameStart_GameStateText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* GameStart_DefenceConnectText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* GameStart_OffenceConnectText;
	// Game Start

	// Game End
	UPROPERTY(meta = (BindWidget))
	UTextBlock* GameEnd_FirstPlayerText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* GameEnd_SecondPlayerText;


	UPROPERTY(meta = (BindWidget))
	UTextBlock* GameEnd_WinnerText;


	UPROPERTY(meta = (BindWidget))
	UImage* GameEnd_NexusAmount_BackGround;

	UPROPERTY(meta = (BindWidget))
	UImage* GameEnd_NexusHealth_BackGround;

	UPROPERTY(meta = (BindWidget))
	UImage* GameEnd_TimeLeft_BackGround;
	

	UPROPERTY(meta = (BindWidget))
	UTextBlock* FirtsPlayerNexus_AmountText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* FirtsPlayerNexus_HealthText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* FirtsPlayer_TimeLeftText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* SecondPlayerNexus_AmountText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* SecondPlayerNexus_HealthText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* SecondPlayer_TimeLeftText;


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

	UFUNCTION(BlueprintCallable)
	void SetConnectState(bool offence, bool defence);



	UFUNCTION(BlueprintCallable)
	void SetFinalResultPanel(int32 FirstNexusCount, int32 FirstNexusHealth, int32 FirstTimeLeft, int32 SecondNexusCount, int32 SecondNexusHealth, int32 SecondTimeLeft);

	UFUNCTION(BlueprintCallable)
	void CheckWhoIsWinner(int32 FirstNexusCount, int32 FirstNexusHealth, int32 FirstTimeLeft, int32 SecondNexusCount, int32 SecondNexusHealth, int32 SecondTimeLeft);

};
