// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "XRDefenceEnums.h"
#include "XRGamePlayMode.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCustomEvent, bool, isGrab, EObjectType, objectType);

class AXR_Character;

UCLASS()
class XRDEFENCEGAME_API AXRGamePlayMode : public AGameMode
{
	GENERATED_BODY()

 public:

    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnCustomEvent OnObjectGrabEvent;

    UFUNCTION(BlueprintCallable, Category = "Events")
    void TriggerOnObjectGrabEvent(bool isGrab, EObjectType objectType);

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
    AXR_Character* PoolSpawnActor(ECharacterType characterType);

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
    void SpawnActorForUpgrade(ECharacterType characterType, FVector position, FRotator rotation, FCharacterValueTransmitForm form);

};
