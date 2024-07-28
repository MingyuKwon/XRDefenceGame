// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "MapLocationSetGameMode.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMapRotateEvent, float, RotateAmount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMapLocationEvent, FVector, SpawnLocation);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMapSpawnEvent);

UCLASS()
class XRDEFENCEGAME_API AMapLocationSetGameMode : public AGameMode
{
	GENERATED_BODY()
	
public:
    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnMapRotateEvent OnMapRotateEvent;

    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnMapLocationEvent OnMapLocationEvent;

    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnMapSpawnEvent OnMapSpawnEvent;

    UFUNCTION(BlueprintCallable, Category = "Events")
    void TriggerOnMapRotateEvent(float RotateAmount);

    UFUNCTION(BlueprintCallable, Category = "Events")
    void TriggerOnMapLocationEvent(FVector SpawnLocation);

    UFUNCTION(BlueprintCallable, Category = "Events")
    void TriggerOnMapSpawnEvent();

protected:

    virtual void BeginPlay() override;

    virtual void PostLogin(APlayerController* NewPlayer) override;

    void PostTravelSetPlayerLocation();


};