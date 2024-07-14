// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/Offence/XR_CharacterOffence.h"
#include "MyXR_CharacterOffenceBuff.generated.h"

/**
 * 
 */
UCLASS()
class XRDEFENCEGAME_API AMyXR_CharacterOffenceBuff : public AXR_CharacterOffence
{
	GENERATED_BODY()
	

protected:
	virtual void OnBoardCalledFunction(bool isOnBoard, bool isSpawnedByHand) override;

	virtual void BehaviorAvailableTimerFunction() override;

	UFUNCTION()
	virtual void BuffTimerFunction();

	UFUNCTION()
	virtual void ApplyBuffInRange();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Niagara Parameter")
	class UNiagaraSystem* BuffRangeNiagara;


	FTimerHandle BuffTimerHandle;

	UPROPERTY(EditAnywhere, Category = "Buff Parameter")
	float BuffDelay = 5.f;

	UPROPERTY(EditAnywhere, Category = "Buff Parameter")
	float BuffRadius = 10.f;


};
