// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "XRDefenceEnums.h"
#include "PlayerPawn.generated.h"

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


	UPROPERTY(BlueprintReadWrite)
	FVector LeftHandPosition;
	UPROPERTY(BlueprintReadWrite)
	FVector RightHandPosition;

	UFUNCTION(BlueprintImplementableEvent)
	void SetLeftHandPosition();
	UFUNCTION(BlueprintImplementableEvent)
	void SetRightHandPosition();

	UFUNCTION()
	inline FVector GetLeftHandPosition() { SetLeftHandPosition(); return LeftHandPosition; }
	UFUNCTION()
	inline FVector GetRightHandPosition() { SetRightHandPosition(); return RightHandPosition; }

};
