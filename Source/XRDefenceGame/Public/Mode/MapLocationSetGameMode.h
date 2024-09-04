// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "MapLocationSetGameMode.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMapRotateEvent, float, RotateAmount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMapLocationEvent, FVector, SpawnLocation);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMapSpawnEvent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOFindSessionFailedEvent);


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

    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOFindSessionFailedEvent OFindSessionFailedEvent;

    UFUNCTION(BlueprintCallable, Category = "Events")
    void TriggerOnMapRotateEvent(float RotateAmount);

    UFUNCTION(BlueprintCallable, Category = "Events")
    void TriggerOnMapLocationEvent(FVector SpawnLocation);

    UFUNCTION(BlueprintCallable, Category = "Events")
    void TriggerOnMapSpawnEvent();

    UFUNCTION(BlueprintCallable, Category = "Travel")
    void MoveToLobby();

    UPROPERTY(BlueprintReadWrite)
    bool bOpenLevelServer = true;

protected:

    virtual void BeginPlay() override;

    virtual void PostLogin(APlayerController* NewPlayer) override;

    UPROPERTY(EditAnywhere)
    FString LobbyMapName;

    class UMultiplayerSessionsSubsystem* MultiplayerSessionsSubsystem;

    void InitializeOnlineSubSystem();

    UFUNCTION(BlueprintCallable)
    void CreateSessionThroughSubSystem();
    UFUNCTION(BlueprintCallable)
    void JoinSessionThroughSubSystem();

    UFUNCTION()
    void OnCreateSession(bool bwasSuccessFul);
    void OnFindSession(const TArray<FOnlineSessionSearchResult>& SessionResult, bool bWasSuccessFul);
    void OnJoinSession(EOnJoinSessionCompleteResult::Type Result);
    UFUNCTION()
    void OnDestroySession(bool bwasSuccessFul);
    void OnStartSession(bool bwasSuccessFul);

    class UXRDefenceGameInstance* XRGameInstace;


};
