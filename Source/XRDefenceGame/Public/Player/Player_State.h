// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "Player_State.generated.h"

/**
 * 
 */
UCLASS()
class XRDEFENCEGAME_API APlayer_State : public APlayerState
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "PlayerState")
	void SetGold(float NewGold);

	UFUNCTION(BlueprintCallable, Category = "PlayerState")
	float GetGold() ;

	UFUNCTION(BlueprintCallable, Category = "PlayerState")
	void UpgradeMaxGold(bool bIncrease);

	UFUNCTION(BlueprintCallable, Category = "PlayerState")
	float GetMaxGold();


protected:
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "PlayerState")
	float CurrentGold = 0.f;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "PlayerState")
	float MaxGold = 30.f;

	float MaxGoldUpgradeUnit = 10.f;


};
