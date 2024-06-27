// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "XRDefenceEnums.h"
#include "XRGamePlayMode.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCustomEvent, bool, isGrab, EObjectType, objectType);

UCLASS()
class XRDEFENCEGAME_API AXRGamePlayMode : public AGameMode
{
	GENERATED_BODY()

 public:

    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnCustomEvent OnObjectGrabEvent;

    UFUNCTION(BlueprintCallable, Category = "Events")
    void TriggerOnObjectGrabEvent(bool isGrab, EObjectType objectType);
    
};