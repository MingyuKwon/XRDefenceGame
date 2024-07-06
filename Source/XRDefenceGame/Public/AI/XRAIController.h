// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "XRAIController.generated.h"

/**
 * 
 */
class UBehaviorTreeComponent;


UCLASS()
class XRDEFENCEGAME_API AXRAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	AXRAIController(const FObjectInitializer& ObjectInitializer);

protected:

	UPROPERTY()
	UBehaviorTreeComponent* BehaviorTreeComponent;

	class UCrowdFollowingComponent* CrowdComp;
};
