// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/Player_Controller.h"
#include "Player/PlayerPawn.h"


void APlayer_Controller::UpdateCurrentRightPose(Pose inputPose)
{
	// playerPawn이 null인 경우에만 새로 가져와서 할당을 하고, 아니라면 기존 것 쓰는 것으로 최적화
	playerPawn = (playerPawn == nullptr) ? Cast<APlayerPawn>(GetPawn()) : playerPawn;

	if (playerPawn == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("APlayer_Controller::UpdateCurrentPose : playerPawn == nullptr"));
		return;
	}

	currentRightPose = inputPose;
	playerPawn->PoseRightAction(currentRightPose);

}

void APlayer_Controller::UpdateCurrentLeftPose(Pose inputPose)
{
	// playerPawn이 null인 경우에만 새로 가져와서 할당을 하고, 아니라면 기존 것 쓰는 것으로 최적화
	playerPawn = (playerPawn == nullptr) ? Cast<APlayerPawn>(GetPawn()) : playerPawn;

	if (playerPawn == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("APlayer_Controller::UpdateCurrentPose : playerPawn == nullptr"));
		return;
	}

	currentLeftPose = inputPose;
	playerPawn->PoseLeftAction(currentLeftPose);
}
