// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/Offence/MyXR_CharacterOffenceBattle.h"
#include "MyXR_CharacterOffenceBuff.generated.h"

/**
 * 
 */
UCLASS()
class XRDEFENCEGAME_API AMyXR_CharacterOffenceBuff : public AMyXR_CharacterOffenceBattle
{
	GENERATED_BODY()
	

protected:
	virtual void OnBoardCalledFunctionServer(bool isOnBoard, bool isSpawnedByHand) override;

	virtual void BehaviorAvailableTimerFunction() override;

	UFUNCTION()
	virtual void ApplyBuffInRange();

	UFUNCTION(NetMulticast, Reliable)
	void SpawnBuffRange();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Niagara Parameter")
	class UNiagaraSystem* BuffRangeNiagara;




	FTimerHandle BuffTimerHandle;

	UPROPERTY(EditAnywhere, Category = "Buff Parameter")
	float BuffDelay = 5.f;

	UPROPERTY(EditAnywhere, Category = "Buff Parameter")
	float BuffRadius = 10.f;

	UPROPERTY(EditAnywhere, Category = "Buff Parameter")
	float HealAmount = 50.f;

	UPROPERTY(EditAnywhere, Category = "Buff Parameter")
	float BuffAmount = 10.f;

	UPROPERTY(EditAnywhere, Category = "Sound Parameter")
	USoundBase* SoundBuffRange;


	UFUNCTION()
	virtual TArray<AXR_Character*> FindBuffableCharacter();

};
