// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "XRDefenceEnums.h"
#include "XRGamePlayMode.generated.h"

class AXR_Character;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnNexusDamageEvent, ENexusType, nexusType, float, currentHealth);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCustomEvent, bool, isGrab, EObjectType, objectType);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCostEvent, EObjectType, objectType, float, cost);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnChrarcterDieEvent, AXR_Character*, DieCharacter);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnGoldMineBroadCastEvent, EObjectType, objectType, bool, bRemove, float, perSecGold);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnChrarcterSpawnEvent, FVector , SpawnLocation);



UCLASS()
class XRDEFENCEGAME_API AXRGamePlayMode : public AGameMode
{
	GENERATED_BODY()

 public:

    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnCustomEvent OnObjectGrabEvent;

    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnChrarcterSpawnEvent OnCharacterSpawnEvent;

    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnCostEvent OnCostEvent;

    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnNexusDamageEvent OnNexusDamageEvent;

    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnChrarcterDieEvent OnChrarcterDieEvent;

    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnGoldMineBroadCastEvent OnGoldMineBroadCastEvent;


    UFUNCTION(BlueprintCallable, Category = "Events")
    void TriggerOnObjectGrabEvent(bool isGrab, EObjectType objectType);

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
    AXR_Character* PoolSpawnActor(ECharacterType characterType);

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
    void SpawnActorForUpgrade(ECharacterType characterType, FVector position, FRotator rotation, FCharacterValueTransmitForm form);

};
