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

	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* GoldPile;

	virtual void OnBoardCalledFunction(bool isOnBoard, bool isSpawnedByHand) override;

	virtual void DissolveCallBack(float percent) override;

	virtual void DissolveCallBackReverse(float percent) override;

	virtual void Death(bool bDieInTrash) override;

	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "GoldMine Parameter")
	float PerSecGold = 1.f;

	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "GoldMine Parameter")
	float LifTime = 10.f;

	float TimeCount = -1.f;

	virtual void BroadCastGoldMineOnBoard(bool flag);

	UFUNCTION()
	virtual void BroadCastGoldMineTick();

	FTimerHandle GoldMineTimerHandler;


};
