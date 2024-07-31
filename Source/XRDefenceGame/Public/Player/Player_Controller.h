// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "XRDefenceEnums.h"
#include "Interface/HandInteractInterface.h"
#include "XRDefenceEnums.h"
#include "Player_Controller.generated.h"

/**
 * 
 */
UCLASS()
class XRDEFENCEGAME_API APlayer_Controller : public APlayerController
{
	GENERATED_BODY()
	

public:
	APlayer_Controller();

	virtual void Tick(float DeltaTime) override;
	virtual void BeginPlay() override;


	UFUNCTION(BlueprintCallable)
	void UpdateCurrentRightPose(Pose inputPose);

	UFUNCTION(BlueprintCallable)
	void UpdateCurrentLeftPose(Pose inputPose);

	UFUNCTION(BlueprintCallable)
	void UpdateCurrentRightGesture(EGesture inputGesture);

	UFUNCTION(BlueprintCallable)
	void ShouldRightGestureRelease(Pose inputPose);


	UFUNCTION(BlueprintCallable)
	void HandInteractRightOverlapStart(TScriptInterface<IHandInteractInterface> handInteractInterface);

	UFUNCTION(BlueprintCallable)
	void HandInteractRightOverlapEnd(TScriptInterface<IHandInteractInterface> handInteractInterface);

	UFUNCTION(BlueprintCallable)
	void HandInteractLeftOverlapStart(TScriptInterface<IHandInteractInterface> handInteractInterface);

	UFUNCTION(BlueprintCallable)
	void HandInteractLeftOverlapEnd(TScriptInterface<IHandInteractInterface> handInteractInterface);

	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
	void UpdateUserHandUI();

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TObjectPtr<class AXRGamePlayMode> XRGamePlayMode;

	UFUNCTION(BlueprintCallable)
	void SetControllerObjectType(EObjectType objectType);


	UPROPERTY(BlueprintReadWrite, EditAnyWhere, Replicated, Category = "Pawn Parameter")
	EObjectType controllerObjectType;

	FTimerHandle CanFireTimerHandle;

	UPROPERTY(BlueprintReadWrite)
	bool bcanFire = false;

	UFUNCTION(BlueprintCallable)
	void CannotFire();


	FTimerHandle DebugHandle;


	FTimerHandle CanSpeedBuffTimerHandle;

	UPROPERTY(BlueprintReadWrite)
	bool bcanSpeedBuff = false;

	UFUNCTION(BlueprintCallable)
	void CannotBuff();

	// Server
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;



private:

	// This will be saved in GameMode
	float orangeNexusHealth = 1000.f;
	float purpleNexusHealth = 1000.f;
	float blueNexusHealth = 1000.f;

	float curerntLeftTime = 300.f;

	UFUNCTION(BlueprintCallable)
	void GoldMineBroadCastCallBack(EObjectType objectType, bool bRemove, float perSecGold);

	UFUNCTION(BlueprintCallable)
	void NexusHealthChange(ENexusType nexusType, float currentHealth);


	void ReleaseRightInteract(TScriptInterface<IHandInteractInterface> handInteractInterface);
	void ReleaseLeftInteract(TScriptInterface<IHandInteractInterface> handInteractInterface);

	bool GetPlayerPawn();
	bool GetPlayer_State();


	Pose currentRightPose;
	Pose currentLeftPose;

	EGesture currentRightGesture = EGesture::None;


	TScriptInterface<IHandInteractInterface> currentRightInteractInterface = nullptr;
	TScriptInterface<IHandInteractInterface> currentLeftInteractInterface = nullptr;

	bool bLeftGrabbing = false;
	bool bRightGrabbing = false;

	inline bool IsRightGrabable() { return currentRightInteractInterface && !IHandInteractInterface::Execute_IsOnBoard(currentRightInteractInterface.GetObject()); }
	inline bool IsLeftGrabable() { return currentLeftInteractInterface && !IHandInteractInterface::Execute_IsOnBoard(currentLeftInteractInterface.GetObject()); }
	inline bool IsRightGrabable_CostInclude() { return IsRightGrabable() && !IHandInteractInterface::Execute_GetDisableHighLight(currentRightInteractInterface.GetObject()); }
	inline bool IsLeftGrabable_CostInclude() { return IsLeftGrabable() && !IHandInteractInterface::Execute_GetDisableHighLight(currentLeftInteractInterface.GetObject()); }

	void LeftGrabStart();
	void RightGrabStart();
	void LeftGrabEnd();
	void RightGrabEnd();

	UFUNCTION(Server, Reliable)
	void Server_InteractableEffectStart(int32 NetWorkID);

	class APlayerPawn* playerPawn = nullptr;
	class APlayer_State* playerState = nullptr;

	UFUNCTION()
	virtual void DefaultGoldEarn();

	UFUNCTION()
	void GestureCoolTimeTick();

	UFUNCTION()
	virtual void GoldCostEventCallBack(EObjectType objectType, float cost);


	UFUNCTION()
	virtual void OnGameStart();

	UFUNCTION()
	virtual void OnGameEnd();

	UFUNCTION()
	virtual void OnGameTimerShow(float leftSecond);


	FTimerHandle DefaultGoldTimerHandle;
	void StartDefaultTimeTick();
	bool CanAffordCost(float Cost);


	FTimerHandle GestureCoolTimeTimeHandle;
	int32 GestureCoolTime = 5;
	int32 GestureCoolTimeUnit = 5;

public:

	UFUNCTION(BlueprintCallable)
	int32 GetGestureCoolTime() { return GestureCoolTime; }

	UFUNCTION(BlueprintCallable)
	bool IsGestureWorkable() { return GestureCoolTime == 0; }

	UFUNCTION(BlueprintCallable)
	void SetGestureCoolTime() { GestureCoolTime = GestureCoolTimeUnit; }

};
