// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "XRDefenceEnums.h"
#include "PlayerPawn.generated.h"

class AXR_Character;
UCLASS()
class XRDEFENCEGAME_API APlayerPawn : public APawn
{
	GENERATED_BODY()

public:
	APlayerPawn();

	UFUNCTION(BlueprintImplementableEvent)
	void PoseRightAction(Pose currentPose);

	UFUNCTION(BlueprintImplementableEvent)
	void PoseLeftAction(Pose currentPose);

	UFUNCTION(BlueprintImplementableEvent)
	void GestureRightAction(EGesture currentGesture);

	UFUNCTION(BlueprintImplementableEvent)
	void ReleaseGestureRight(EGesture currentGesture);

	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
	void SetPawnTransformForGameStart();

	UFUNCTION(BlueprintCallable)
	TArray<AXR_Character*> GetRangeCharacters(FVector impactPoint, float radius, EObjectType objectype);


	UPROPERTY(BlueprintReadWrite)
	FVector LeftHandPosition;
	UPROPERTY(BlueprintReadWrite)
	FVector RightHandPosition;

	UFUNCTION(BlueprintImplementableEvent)
	void SetLeftHandPosition();
	UFUNCTION(BlueprintImplementableEvent)
	void SetRightHandPosition();


	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
	void UpdateUserLeftHandUI(float GoldAmount, float MaxGoldAmount, float TimeSecond, float TotalHealthAmount, float OrangeHealthAmount, float BlueHealthAmount, float PurpleHealthAmount, float GesturePercent);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetUIGoldAmount(float GoldAmount, float MaxGoldAmount);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetUITime(float TimeSecond);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetUIHealth(float HealthAmount);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetUIOrnageHealth(float HealthAmount);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetUBlueHealth(float HealthAmount);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetUIPurpleHealth(float HealthAmount);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetUIGestureCoolTime(float percent);


	UFUNCTION()
	inline FVector GetLeftHandPosition() { SetLeftHandPosition(); return LeftHandPosition; }
	UFUNCTION()
	inline FVector GetRightHandPosition() { SetRightHandPosition(); return RightHandPosition; }


};
