// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/Player_Controller.h"
#include "Player/PlayerPawn.h"


void APlayer_Controller::Tick(float DeltaTime)
{
	if (!GetPlayerPawn()) return;

	if (bRightGrabbing)
	{
		if (IsRightGrabable())
		{
			IHandInteractInterface::Execute_SetInteractPosition(currentRightInteractInterface.GetObject(), playerPawn->GetRightHandPosition());
		}
	}

	if (bLeftGrabbing)
	{
		if (IsLeftGrabable())
		{
			IHandInteractInterface::Execute_SetInteractPosition(currentLeftInteractInterface.GetObject(), playerPawn->GetLeftHandPosition());
		}
	}
}

bool APlayer_Controller::GetPlayerPawn()
{
	playerPawn = (playerPawn == nullptr) ? Cast<APlayerPawn>(GetPawn()) : playerPawn;

	if (playerPawn == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("APlayer_Controller::GetPlayerPawn : playerPawn == nullptr"));
		return false;
	}

	return true;
}

void APlayer_Controller::UpdateCurrentRightPose(Pose inputPose)
{
	if (!GetPlayerPawn()) return;

	currentRightPose = inputPose;
	playerPawn->PoseRightAction(currentRightPose);

	switch (inputPose)
	{
		case Pose::Grab :
			RightGrabStart();
			break;

		default:
			RightGrabEnd();
			break;
	}

}

void APlayer_Controller::UpdateCurrentLeftPose(Pose inputPose)
{
	if (!GetPlayerPawn()) return;

	currentLeftPose = inputPose;
	playerPawn->PoseLeftAction(currentLeftPose);

	switch (inputPose)
	{
		case Pose::Grab:
			LeftGrabStart();
			break;

		default:
			LeftGrabEnd();
			break;
	}
}

void APlayer_Controller::HandInteractRightOverlapStart(TScriptInterface<IHandInteractInterface> handInteractInterface)
{
	if (bRightGrabbing) return;

	if (currentLeftInteractInterface == handInteractInterface) return;
    if (currentRightInteractInterface == handInteractInterface) return;

    if (currentRightInteractInterface)
    {
        IHandInteractInterface::Execute_InteractableEffectEnd(currentRightInteractInterface.GetObject());
    }

    if (handInteractInterface)
    {
        IHandInteractInterface::Execute_InteractableEffectStart(handInteractInterface.GetObject());
    }

    currentRightInteractInterface = handInteractInterface;
}

void APlayer_Controller::HandInteractRightOverlapEnd(TScriptInterface<IHandInteractInterface> handInteractInterface)
{
	if (bRightGrabbing) return;
    if (currentRightInteractInterface != handInteractInterface) return;

    if (currentRightInteractInterface)
    {
        IHandInteractInterface::Execute_InteractableEffectEnd(currentRightInteractInterface.GetObject());
    }

    currentRightInteractInterface = nullptr;
}

void APlayer_Controller::HandInteractLeftOverlapStart(TScriptInterface<IHandInteractInterface> handInteractInterface)
{
	if (bLeftGrabbing) return;

	if (currentLeftInteractInterface == handInteractInterface) return;
	if (currentRightInteractInterface == handInteractInterface) return;

    if (currentLeftInteractInterface)
    {
        IHandInteractInterface::Execute_InteractableEffectEnd(currentLeftInteractInterface.GetObject());
    }

    if (handInteractInterface)
    {
        IHandInteractInterface::Execute_InteractableEffectStart(handInteractInterface.GetObject());
    }

    currentLeftInteractInterface = handInteractInterface;
}

void APlayer_Controller::HandInteractLeftOverlapEnd(TScriptInterface<IHandInteractInterface> handInteractInterface)
{
	if (bLeftGrabbing) return;
    if (currentLeftInteractInterface != handInteractInterface) return;

    if (currentLeftInteractInterface)
    {
        IHandInteractInterface::Execute_InteractableEffectEnd(currentLeftInteractInterface.GetObject());
    }

    currentLeftInteractInterface = nullptr;
}


void APlayer_Controller::LeftGrabStart()
{
	if (bLeftGrabbing) return;

	if (IsLeftGrabable())
	{
		IHandInteractInterface::Execute_GrabStart(currentLeftInteractInterface.GetObject());
	}

	bLeftGrabbing = true;

}

void APlayer_Controller::LeftGrabEnd()
{
	if (!bLeftGrabbing) return;

	if (IsLeftGrabable())
	{
		IHandInteractInterface::Execute_GrabEnd(currentLeftInteractInterface.GetObject());
	}

	bLeftGrabbing = false;

}  

void APlayer_Controller::RightGrabStart()
{
	if (bRightGrabbing) return;

	if (IsRightGrabable())
	{
		IHandInteractInterface::Execute_GrabStart(currentRightInteractInterface.GetObject());
	}


	bRightGrabbing = true;

}

void APlayer_Controller::RightGrabEnd()
{
	if (!bRightGrabbing) return;

	if (IsRightGrabable())
	{
		IHandInteractInterface::Execute_GrabEnd(currentRightInteractInterface.GetObject());
	}

	bRightGrabbing = false;

}

