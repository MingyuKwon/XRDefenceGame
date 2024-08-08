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

	UFUNCTION(BlueprintCallable)
	void UpdateUserHandUI();

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TObjectPtr<class AXRGamePlayMode> XRGamePlayMode;

	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
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
	
	UPROPERTY(BlueprintReadWrite, EditAnyWhere, Replicated, Category = "Pawn Parameter")
	bool bGamePlaying = false;


	FTimerHandle GestureCoolTimeTimeHandle;
	UPROPERTY(BlueprintReadWrite, EditAnyWhere, Replicated, Category = "Pawn Parameter")
	int32 GestureCoolTime = 5;
	int32 GestureCoolTimeUnit = 5;

private:

	UFUNCTION(BlueprintCallable)
	void GoldMineBroadCastCallBack(EObjectType objectType, bool bRemove, float perSecGold);

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


	bool IsInteractActorMine(TScriptInterface<IHandInteractInterface> interact);

	UFUNCTION(Server, Reliable)
	void Server_InteractableEffectStart(int32 NetWorkID);
	void InteractableEffectStart(int32 NetWorkID);
	void TryInteractableEffectStart(int32 NetWorkID);

	UFUNCTION(Server, Reliable)
	void Server_InteractableEffecEnd(int32 NetWorkID);
	void InteractableEffectEnd(int32 NetWorkID);
	void TryInteractableEffectEnd(int32 NetWorkID);

	UFUNCTION(Server, Reliable)
	void Server_GrabStart(int32 NetWorkID);
	void GrabStart(int32 NetWorkID);
	void TryGrabStart(int32 NetWorkID);

	UFUNCTION(Server, Reliable)
	void Server_GrabEnd(int32 NetWorkID);
	void GrabEnd(int32 NetWorkID);
	void TryGrabEnd(int32 NetWorkID);

	UFUNCTION(Server, Reliable)
	void Server_SetInteractPosition(int32 NetWorkID, FVector Position);
	void SetInteractPosition(int32 NetWorkID, FVector Position);
	void TrySetInteractPosition(int32 NetWorkID, FVector Position);

	UFUNCTION(Server, Reliable)
	void Server_SetDisableHighLight(int32 NetWorkID, bool bDiable);
	void SetDisableHighLight(int32 NetWorkID, bool bDiable);
	void TrySetDisableHighLight(int32 NetWorkID, bool bDiable);


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

	FTimerHandle DefaultGoldTimerHandle;
	void StartDefaultTimeTick();
	bool CanAffordCost(float Cost);

public:

	UFUNCTION(BlueprintCallable)
	int32 GetGestureCoolTime() { return GestureCoolTime; }

	UFUNCTION(BlueprintCallable)
	bool IsGestureWorkable() { return GestureCoolTime == 0; }

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void SetGestureCoolTime();

};
