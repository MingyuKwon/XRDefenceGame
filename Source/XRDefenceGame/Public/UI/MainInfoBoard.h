// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "XRDefenceEnums.h"
#include "MainInfoBoard.generated.h"

UCLASS()
class XRDEFENCEGAME_API AMainInfoBoard : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMainInfoBoard();


	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
	void UpdateUI(float TimeSecond, float TotalHealthAmount, float OrangeHealthAmount, float BlueHealthAmount, float PurpleHealthAmount);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetUITime(float TimeSecond);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetUIHealth(float HealthAmount);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetUIOrnageHealth(float HealthAmount);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetUBlueHealth(float HealthAmount);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetUIPurpleHealth(float HealthAmount);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetgameStart_GameStateText(FString& text);


	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
	void WhichPanelToShow_Multi(EGameMatchState matchState);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void WhichPanelToShow(EGameMatchState matchState);





protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnGameStart();
	UFUNCTION()
	void OnGameEnd();

	UFUNCTION(BlueprintCallable)
	void NexusHealthChange(ENexusType nexusType, float currentHealth);

	UFUNCTION()
	virtual void OnGameTimerShow(float leftSecond);

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TObjectPtr<class AXRGamePlayMode> XRGamePlayMode;



private:
	float orangeNexusHealth = 1000.f;
	float purpleNexusHealth = 1000.f;
	float blueNexusHealth = 1000.f;

	float curerntLeftTime = 240.f;





public:	
	virtual void Tick(float DeltaTime) override;

};
