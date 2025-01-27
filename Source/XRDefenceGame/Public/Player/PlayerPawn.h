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

	UFUNCTION(BlueprintCallable)
	void SetPawnTransformForGameStart();

	UFUNCTION(BlueprintCallable)
	TArray<AXR_Character*> GetRangeCharacters(FVector impactPoint, float radius, EObjectType objectype);

	UPROPERTY(EditAnywhere)
	bool bDefaultPawn = true;


	UPROPERTY(BlueprintReadWrite)
	FVector LeftHandPosition;
	UPROPERTY(BlueprintReadWrite)
	FVector RightHandPosition;

	UFUNCTION(BlueprintImplementableEvent)
	void SetLeftHandPosition();
	UFUNCTION(BlueprintImplementableEvent)
	void SetRightHandPosition();


	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
	void UpdateUserLeftHandUI(float GoldAmount, float MaxGoldAmount, float GoldMineCount, float GesturePercent);

	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
	void SetUIOffenceDefence_Multi(EObjectType objectType);


	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetUIGoldAmount(float GoldAmount, float MaxGoldAmount, float GoldMineCount);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetUIGestureCoolTime(float percent);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetUIOffenceDefence(EObjectType objectType);


	UFUNCTION()
	inline FVector GetLeftHandPosition() { SetLeftHandPosition(); return LeftHandPosition; }
	UFUNCTION()
	inline FVector GetRightHandPosition() { SetRightHandPosition(); return RightHandPosition; }

protected:
	virtual void BeginPlay() override;

	UFUNCTION(Server, Reliable)
	void ServerGameModeCallPositionReady();

	class APlayer_Controller* PlayerController;
	class AXRGamePlayMode* XRGamePlayMode;

	UFUNCTION(BlueprintCallable, Category = "Events")
	void TriggerSurrender();

	UFUNCTION(BlueprintCallable, Category = "Events", Server, Reliable)
	void TriggerSurrender_Server(bool bServer);


};
