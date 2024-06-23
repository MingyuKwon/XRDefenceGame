// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/Player_Controller.h"
#include "Player/PlayerPawn.h"


void APlayer_Controller::UpdateCurrentRightPose(Pose inputPose)
{
	// playerPawn�� null�� ��쿡�� ���� �����ͼ� �Ҵ��� �ϰ�, �ƴ϶�� ���� �� ���� ������ ����ȭ
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
	// playerPawn�� null�� ��쿡�� ���� �����ͼ� �Ҵ��� �ϰ�, �ƴ϶�� ���� �� ���� ������ ����ȭ
	playerPawn = (playerPawn == nullptr) ? Cast<APlayerPawn>(GetPawn()) : playerPawn;

	if (playerPawn == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("APlayer_Controller::UpdateCurrentPose : playerPawn == nullptr"));
		return;
	}

	currentLeftPose = inputPose;
	playerPawn->PoseLeftAction(currentLeftPose);
}

void APlayer_Controller::HandInteractRightOverlapStart(TScriptInterface<IHandInteractInterface> handInteractInterface)
{
	if (currentRightInteractInterface == handInteractInterface) return;

	currentRightInteractInterface->InteractableEffectEnd();
	handInteractInterface->InteractableEffectStart();
	currentRightInteractInterface = handInteractInterface;

}

void APlayer_Controller::HandInteractRightOverlapEnd(TScriptInterface<IHandInteractInterface> handInteractInterface)
{
	if (currentRightInteractInterface != handInteractInterface) return;

	currentRightInteractInterface->InteractableEffectEnd();
	currentRightInteractInterface = nullptr;

}

void APlayer_Controller::HandInteractLeftOverlapStart(TScriptInterface<IHandInteractInterface> handInteractInterface)
{
	if (currentLeftInteractInterface == handInteractInterface) return;

	currentLeftInteractInterface->InteractableEffectEnd();
	handInteractInterface->InteractableEffectStart();
	currentLeftInteractInterface = handInteractInterface;
}

void APlayer_Controller::HandInteractLeftOverlapEnd(TScriptInterface<IHandInteractInterface> handInteractInterface)
{
	if (currentLeftInteractInterface != handInteractInterface) return;

	currentLeftInteractInterface->InteractableEffectEnd();
	currentLeftInteractInterface = nullptr;

}
