// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/Offence/XR_CharacterOffence.h"
#include "MyXR_CharacterOffenceBattle.generated.h"

/**
 * 
 */
UCLASS()
class XRDEFENCEGAME_API AMyXR_CharacterOffenceBattle : public AXR_CharacterOffence
{
	GENERATED_BODY()
	
public:
	virtual void CharacterActionImpact() override;

	virtual void FindNearbyEnemy(AXR_Character*& outFirstNear, AXR_Character*& outSecondNear) override;

	void SetOffenceTarget();

protected:
	virtual void CharacterActionStart() override;

	virtual void OtherCharacterSpawnCallBack(FVector spawnLocation) override;

	virtual void OnBoardCalledFunction(bool isOnBoard, bool isSpawnedByHand) override;

	virtual void BehaviorAvailableTimerFunction() override;

	virtual void TargetDieCallBack(AXR_Character* DieTarget) override;

};
