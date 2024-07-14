// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/Offence/XR_CharacterOffence.h"
#include "MyXR_CharacterOffenceSpawn.generated.h"

/**
 * 
 */
UCLASS()
class XRDEFENCEGAME_API AMyXR_CharacterOffenceSpawn : public AXR_CharacterOffence
{
	GENERATED_BODY()
	
protected:
	UFUNCTION()
	virtual void SpawnTimerFunction();

	FTimerHandle SpawnTimerHandle;

	FTimerHandle Spawn2TimerHandle;

	FTimerHandle Spawn3TimerHandle;

	UFUNCTION()
	void SpawnCharacter();


	virtual void OnBoardCalledFunction(bool isOnBoard, bool isSpawnedByHand) override;

	virtual void BehaviorAvailableTimerFunction() override;


	UPROPERTY(EditAnywhere, Category = "Spawn Parameter")
	float SpawnDelay = 7.f;

	UPROPERTY(EditAnywhere, Category = "Spawn Parameter")
	int32 SpawnCount = 3.f;

	UPROPERTY(EditAnywhere, Category = "Spawn Parameter")
	TSubclassOf<AXR_Character> SpawnClass;

};
