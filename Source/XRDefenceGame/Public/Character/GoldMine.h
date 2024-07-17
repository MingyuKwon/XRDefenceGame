// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/XR_Character.h"
#include "GoldMine.generated.h"

/**
 * 
 */
UCLASS()
class XRDEFENCEGAME_API AGoldMine : public AXR_Character
{
	GENERATED_BODY()
public:
	AGoldMine();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* GoldPile;

	virtual void OnBoardCalledFunction(bool isOnBoard, bool isSpawnedByHand) override;

	virtual void DissolveCallBack(float percent) override;

	virtual void DissolveCallBackReverse(float percent) override;

};
