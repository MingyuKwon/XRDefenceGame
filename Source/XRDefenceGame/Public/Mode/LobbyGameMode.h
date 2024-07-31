// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "LobbyGameMode.generated.h"

/**
 * 
 */
UCLASS()
class XRDEFENCEGAME_API ALobbyGameMode : public AGameMode
{
	GENERATED_BODY()
	
protected:
	virtual void PostLogin(APlayerController* NewPlayer) override;

	FTimerHandle LobbyOutCount;
	void CountDown();
	int32 timeCount = 10;

	int32 playerCount = 0;

	UPROPERTY(EditAnywhere)
	FString GameMapName;





};
