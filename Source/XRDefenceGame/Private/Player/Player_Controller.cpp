// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/Player_Controller.h"
#include "Player/PlayerPawn.h"
#include "Player/Player_State.h"
#include "Kismet/GameplayStatics.h"
#include "Mode/XRGamePlayMode.h"

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

void APlayer_Controller::DefaultGoldEarn()
{
	if (!GetPlayer_State()) return;

	playerState->SetGold(playerState->GetGold() + 1.f);
	UpdateUserHandUI();
}

void APlayer_Controller::StartDefaultGoldEarn()
{
	GetWorld()->GetTimerManager().SetTimer(DefaultGoldTimerHandle, this, &APlayer_Controller::DefaultGoldEarn, 1.f, true);
}

void APlayer_Controller::UpdateUserHandUI()
{
	if (!GetPlayerPawn()) return;
	if (!GetPlayer_State()) return;

	playerPawn->SetUIGoldAmount(playerState->GetGold());
}

void APlayer_Controller::SetControllerObjectType(EObjectType objectType)
{
	controllerObjectType = objectType;
}

void APlayer_Controller::GoldMineBroadCastCallBack(EObjectType objectType, bool bRemove, float perSecGold)
{
	if (controllerObjectType != objectType) return;

	if (perSecGold <= 0) // This is when GoldMine is Set on the Board
	{
		if (bRemove)
		{

		}
		else
		{

		}

		return;
	}

	playerState->SetGold(playerState->GetGold() + perSecGold);
	UpdateUserHandUI();

}

void APlayer_Controller::BeginPlay()
{
	Super::BeginPlay();
	StartDefaultGoldEarn();

	XRGamePlayMode = Cast<AXRGamePlayMode>(UGameplayStatics::GetGameMode(this));
	if (XRGamePlayMode)
	{
		XRGamePlayMode->OnGoldMineBroadCastEvent.AddDynamic(this, &APlayer_Controller::GoldMineBroadCastCallBack);
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

bool APlayer_Controller::GetPlayer_State()
{
	playerState = (playerState == nullptr) ? GetPlayerState<APlayer_State>() : playerState;

	if (playerState == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("APlayer_Controller::GetPlayerState : playerPawn == nullptr"));
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
	if (bRightGrabbing && !IHandInteractInterface::Execute_IsOnBoard(handInteractInterface.GetObject())) return;
	ReleaseRightInteract(handInteractInterface);
}

void APlayer_Controller::ReleaseRightInteract(TScriptInterface<IHandInteractInterface> handInteractInterface)
{
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
	if (bLeftGrabbing && !IHandInteractInterface::Execute_IsOnBoard(handInteractInterface.GetObject())) return;
	ReleaseLeftInteract(handInteractInterface);
}


void APlayer_Controller::ReleaseLeftInteract(TScriptInterface<IHandInteractInterface> handInteractInterface)
{
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
		ReleaseLeftInteract(currentLeftInteractInterface);
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
		ReleaseRightInteract(currentRightInteractInterface);
	}

	bRightGrabbing = false;

}

