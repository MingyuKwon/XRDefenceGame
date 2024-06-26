// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "XRDefenceEnums.h"
#include "Interface/HandInteractInterface.h"
#include "Player_Controller.generated.h"

/**
 * 
 */
UCLASS()
class XRDEFENCEGAME_API APlayer_Controller : public APlayerController
{
	GENERATED_BODY()
	

public:

	virtual void Tick(float DeltaTime) override;


	UFUNCTION(BlueprintCallable)
	void UpdateCurrentRightPose(Pose inputPose);

	UFUNCTION(BlueprintCallable)
	void UpdateCurrentLeftPose(Pose inputPose);


	UFUNCTION(BlueprintCallable)
	void HandInteractRightOverlapStart(TScriptInterface<IHandInteractInterface> handInteractInterface);

	UFUNCTION(BlueprintCallable)
	void HandInteractRightOverlapEnd(TScriptInterface<IHandInteractInterface> handInteractInterface);

	UFUNCTION(BlueprintCallable)
	void HandInteractLeftOverlapStart(TScriptInterface<IHandInteractInterface> handInteractInterface);

	UFUNCTION(BlueprintCallable)
	void HandInteractLeftOverlapEnd(TScriptInterface<IHandInteractInterface> handInteractInterface);

private:
	bool GetPlayerPawn();


	Pose currentRightPose;
	Pose currentLeftPose;

	TScriptInterface<IHandInteractInterface> currentRightInteractInterface = nullptr;
	TScriptInterface<IHandInteractInterface> currentLeftInteractInterface = nullptr;

	bool bLeftGrabbing = false;
	bool bRightGrabbing = false;

	inline bool IsRightGrabable() { return currentRightInteractInterface && !IHandInteractInterface::Execute_IsOnBoard(currentRightInteractInterface.GetObject()); }
	inline bool IsLeftGrabable() { return currentLeftInteractInterface && !IHandInteractInterface::Execute_IsOnBoard(currentLeftInteractInterface.GetObject()); }

	void LeftGrabStart();
	void RightGrabStart();
	void LeftGrabEnd();
	void RightGrabEnd();

	class APlayerPawn* playerPawn = nullptr;

};
