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

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnConnectEvenet, bool, isOffenceConnect, bool, isDefenceConnect);


DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnMapSpawnPawnMoveEvent, EObjectType, objectType, FVector, SpawnLocatoin, FRotator, SpawnRotation);


UCLASS()
class XRDEFENCEGAME_API AXRGamePlayMode : public AGameMode
{
	GENERATED_BODY()

 public:

     TMap<int32, AXR_Character*> ActorMap;
     void AddActorToMap(int32 ActorNetID, AXR_Character* Actor);
     void RemoveActorFromMap(int32 ActorNetID);
     AXR_Character* FindActorInMap(int32 ActorNetID) const;

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

    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnConnectEvenet OnConnectEvenet;

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

    UFUNCTION(BlueprintCallable, Category = "Events")
    void MoveToNextGame();

    UFUNCTION(BlueprintCallable, Category = "Events")
    void TriggerSurrender(bool bServer);



    UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
    AXR_Character* PoolSpawnActor(ECharacterType characterType);

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
    void SpawnActorForUpgrade(ECharacterType characterType, FVector position, FRotator rotation, FCharacterValueTransmitForm form);



    UPROPERTY(EditDefaultsOnly, Category = "Time")
    float GameTimerSecond = 240.f;

    float orangeNexusHealth = 1000.f;
    float purpleNexusHealth = 1000.f;
    float blueNexusHealth = 1000.f;

    int32 OffenceGoldCount = 0;
    int32 DefenceGoldCount = 0;
    void AddGoldCount(EObjectType objectType);


    bool bOffenceConnect = false;
    bool bDefenceConnect = false;
    void TriggerConnectUIUpdate(EObjectType objectType);

    FTimerHandle GameTimerHandle;

    UFUNCTION()
    void GameTimerCallBack();

    FTimerHandle GameStartTimer;

    UFUNCTION()
    void PlayerPositionSetReady();

    UFUNCTION()
    void ShouldGameStart();
    int32 currentconnectPlayer = 0;



    class UMultiplayerSessionsSubsystem* MultiplayerSessionsSubsystem;
    void InitializeOnlineSubSystem();

    UFUNCTION(BlueprintCallable, Category = "Session")
    void DestroyServerSession();

    UFUNCTION()
    void OnDestroySession(bool bwasSuccessFul);

    UPROPERTY(EditAnywhere)
    FString LobbyMapName;

    UPROPERTY(EditAnywhere)
    FString NewmapName;


    bool bSurrenderTriggeredByServer = false;
    bool bSurrenderTriggeredByClient = false;

protected:

    virtual void BeginPlay() override;

    virtual void PostLogin(APlayerController* NewPlayer) override;

    virtual void Logout(AController* Exiting) override;

    class UXRDefenceGameInstance* XRGameInstace;

    void SetGameMatchState(EGameMatchState matchState);

    bool isNowFirstGame();

    FTimerHandle TriggerOnGameStartEventTimerHandle;


    void ResetGameInsatnceValue();




};
