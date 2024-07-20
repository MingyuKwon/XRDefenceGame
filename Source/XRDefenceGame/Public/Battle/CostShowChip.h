// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CostShowChip.generated.h"

UCLASS()
class XRDEFENCEGAME_API ACostShowChip : public AActor
{
	GENERATED_BODY()
	
public:	
	ACostShowChip();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetGoldCostCount(int32 count);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetGoldCostVisible(bool flag);

};
