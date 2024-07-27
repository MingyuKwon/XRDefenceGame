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
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGameStart);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGameEnd);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGameTimerTickEvent, float, leftTime);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnMapSpawnPawnMoveEvent, EObjectType, objectType, FVector, SpawnLocatoin, FRotator, SpawnRotation);


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

    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnGameStart OnGameStart;

    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnGameEnd OnGameEnd;

    UPROPERTY(BlueprintAssignable, Category = "Events")
    FGameTimerTickEvent OnGameTimerTickEvent;

    void PostTravelSetPlayerLocation();

    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnMapSpawnPawnMoveEvent OnMapSpawnPawnMoveEvent;



    UFUNCTION(BlueprintCallable, Category = "Events")
    void TriggerOnMapSpawnPawnMoveEvent(EObjectType objectType, FVector SpawnLocatoin, FRotator SpawnRotation);

    UPROPERTY(BlueprintReadWrite, Category = "Spawn")
    bool bSpawnMapSuccess = false;

    UFUNCTION(BlueprintCallable, Category = "Events")
    void TriggerOnGameStartEvent();

    UFUNCTION(BlueprintCallable, Category = "Events")
    void TriggerOnGameEndEvent();

    UFUNCTION(BlueprintCallable, Category = "Events")
    void TriggerOnNexusDamageEventEvent(ENexusType nexusType, float currentHealth);

    UFUNCTION(BlueprintCallable, Category = "Events")
    void TriggerOnObjectGrabEvent(bool isGrab, EObjectType objectType);



    UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
    AXR_Character* PoolSpawnActor(ECharacterType characterType);

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
    void SpawnActorForUpgrade(ECharacterType characterType, FVector position, FRotator rotation, FCharacterValueTransmitForm form);



    UPROPERTY(EditDefaultsOnly, Category = "Time")
    float GameTimerSecond = 300.f;

    float orangeNexusHealth = 1000.f;
    float purpleNexusHealth = 1000.f;
    float blueNexusHealth = 1000.f;

    

    FTimerHandle GameTimerHandle;

    UFUNCTION()
    void GameTimerCallBack();


    FTimerHandle GameStartTimer;


protected:

    virtual void BeginPlay() override;

    virtual void PostLogin(APlayerController* NewPlayer) override;

};
