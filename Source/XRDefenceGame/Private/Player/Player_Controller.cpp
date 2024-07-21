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

	if (playerState)
	{
		playerState->SetGold(playerState->GetGold() + 1.f);
	}
	UpdateUserHandUI();
}

void APlayer_Controller::GoldCostEventCallBack(EObjectType objectType, float cost)
{
	if (objectType == EObjectType::EOT_OffenceGold && controllerObjectType == EObjectType::EOT_Offence)
	{
		objectType = EObjectType::EOT_Offence;
	}
	else if (objectType == EObjectType::EOT_DeffenceGold && controllerObjectType == EObjectType::EOT_Deffence)
	{
		objectType = EObjectType::EOT_Deffence;
	}
	else if (objectType != controllerObjectType)
	{
		return;
	}

	playerState->SetGold(playerState->GetGold() - cost);

	UpdateUserHandUI();
}

void APlayer_Controller::NexusHealthChange(ENexusType nexusType, float currentHealth)
{
	if (!GetPlayerPawn()) return;
	if (!GetPlayer_State()) return;

	if (nexusType == ENexusType::ENT_NexusPurple)
	{
		purpleNexusHealth = currentHealth;

	}
	else if (nexusType == ENexusType::ENT_NexusOrange)
	{
		orangeNexusHealth = currentHealth;
	}
	else if (nexusType == ENexusType::ENT_NexusBlue)
	{
		blueNexusHealth = currentHealth;

	}

	UpdateUserHandUI();

}

void APlayer_Controller::StartDefaultTimeTick()
{
	// Gold TIck
	GetWorld()->GetTimerManager().SetTimer(DefaultGoldTimerHandle, this, &APlayer_Controller::DefaultGoldEarn, 1.f, true);
	// Gesture CoolTime Tick
	GetWorld()->GetTimerManager().SetTimer(GestureCoolTimeTimeHandle, this, &APlayer_Controller::GestureCoolTimeTick, 1.f, true);

}

bool APlayer_Controller::CanAffordCost(float Cost)
{
	return Cost <= playerState->GetGold();
}

void APlayer_Controller::GestureCoolTimeTick()
{
	if (GestureCoolTime <= 0) return;
	GestureCoolTime = GestureCoolTime - 1;

	UpdateUserHandUI();
}

void APlayer_Controller::UpdateUserHandUI()
{
	if (!GetPlayerPawn()) return;
	if (!GetPlayer_State()) return;

	playerPawn->SetUIGoldAmount(playerState->GetGold(), playerState->GetMaxGold());

	playerPawn->SetUIPurpleHealth(purpleNexusHealth);
	playerPawn->SetUIOrnageHealth(orangeNexusHealth);
	playerPawn->SetUBlueHealth(blueNexusHealth);
	playerPawn->SetUIHealth(purpleNexusHealth + orangeNexusHealth + blueNexusHealth);

	playerPawn->SetUITime(curerntLeftTime);

	playerPawn->SetUIGestureCoolTime(1 - (float)GestureCoolTime / (float)GestureCoolTimeUnit);
	
}

void APlayer_Controller::SetControllerObjectType(EObjectType objectType)
{
	controllerObjectType = objectType;
}

void APlayer_Controller::GoldMineBroadCastCallBack(EObjectType objectType, bool bRemove, float perSecGold)
{
	if (objectType != objectType) return;

	if (perSecGold <= 0) // This is when GoldMine is Set on the Board
	{
		playerState->UpgradeMaxGold(!bRemove);
	}
	else
	{
		playerState->SetGold(playerState->GetGold() + perSecGold);
	}

	UpdateUserHandUI();

}



void APlayer_Controller::BeginPlay()
{
	Super::BeginPlay();

	XRGamePlayMode = Cast<AXRGamePlayMode>(UGameplayStatics::GetGameMode(this));
	if (XRGamePlayMode)
	{
		XRGamePlayMode->OnGoldMineBroadCastEvent.AddDynamic(this, &APlayer_Controller::GoldMineBroadCastCallBack);
		XRGamePlayMode->OnCostEvent.AddDynamic(this, &APlayer_Controller::GoldCostEventCallBack);
		XRGamePlayMode->OnNexusDamageEvent.AddDynamic(this, &APlayer_Controller::NexusHealthChange);

		XRGamePlayMode->OnGameStart.AddDynamic(this, &APlayer_Controller::OnGameStart);
		XRGamePlayMode->OnGameEnd.AddDynamic(this, &APlayer_Controller::OnGameEnd);
		XRGamePlayMode->OnGameTimerTickEvent.AddDynamic(this, &APlayer_Controller::OnGameTimerShow);

	}

}

void APlayer_Controller::OnGameStart()
{
	StartDefaultTimeTick();
}

void APlayer_Controller::OnGameEnd()
{

}

void APlayer_Controller::OnGameTimerShow(float leftSecond)
{
	curerntLeftTime = leftSecond;
	UpdateUserHandUI();
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

void APlayer_Controller::CannotFire()
{
	bcanFire = false;

	GetWorld()->GetTimerManager().ClearTimer(CanFireTimerHandle);

}

void APlayer_Controller::CannotBuff()
{
	bcanSpeedBuff = false;

	GetWorld()->GetTimerManager().ClearTimer(CanSpeedBuffTimerHandle);

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

void APlayer_Controller::UpdateCurrentRightPose(Pose inputPose)
{
	if (!GetPlayerPawn()) return;

	currentRightPose = inputPose;
	playerPawn->PoseRightAction(currentRightPose);

	// This Control Grab
	switch (inputPose)
	{
	case Pose::Grab:
		RightGrabStart();
		break;
	default:
		RightGrabEnd();
		break;
	}

	ShouldRightGestureRelease(inputPose);

}

void APlayer_Controller::ShouldRightGestureRelease(Pose inputPose)
{
	if (currentRightGesture == EGesture::None) return;

	if (currentRightGesture == EGesture::Rock_Scissors)
	{
		if (inputPose != Pose::scissors) {
			playerPawn->ReleaseGestureRight(EGesture::Rock_Scissors);
			currentRightGesture = EGesture::None;

			GetWorld()->GetTimerManager().SetTimer(CanFireTimerHandle, this, &APlayer_Controller::CannotFire, 0.3f, false);

		}
			
	}
	else if (currentRightGesture == EGesture::Rock_Paper)
	{
		if (inputPose != Pose::Paper)
		{
			playerPawn->ReleaseGestureRight(EGesture::Rock_Paper);
			currentRightGesture = EGesture::None;

		}
	}
	else if (currentRightGesture == EGesture::Rock_Thumb)
	{
		if (inputPose != Pose::Thumb)
		{
			playerPawn->ReleaseGestureRight(EGesture::Rock_Thumb);
			currentRightGesture = EGesture::None;

			GetWorld()->GetTimerManager().SetTimer(CanSpeedBuffTimerHandle, this, &APlayer_Controller::CannotBuff, 0.3f, false);

		}
	}


	

}

void APlayer_Controller::UpdateCurrentRightGesture(EGesture inputGesture)
{
	if (!GetPlayerPawn()) return;
	if(inputGesture == EGesture::None) return;


	if (inputGesture == EGesture::Scissors_Thumb )
	{
		if (bcanFire) {
			playerPawn->GestureRightAction(inputGesture);
			bcanFire = false;
			GetWorld()->GetTimerManager().ClearTimer(CanFireTimerHandle);
		}
			
		return;
	}

	if (inputGesture == EGesture::Thumb_Rock)
	{
		if (bcanSpeedBuff) {
			playerPawn->GestureRightAction(inputGesture);
			bcanSpeedBuff = false;
			GetWorld()->GetTimerManager().ClearTimer(CanSpeedBuffTimerHandle);
		}
		return;
	}

	currentRightGesture = inputGesture;

	if (currentRightGesture == EGesture::Rock_Scissors)
	{
		bcanFire = true;
	}

	if (currentRightGesture == EGesture::Rock_Thumb)
	{
		bcanSpeedBuff = true;
	}

	// This differs with pose , because it doesnot trigger every tick except None
	playerPawn->GestureRightAction(inputGesture);
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
		IHandInteractInterface::Execute_SetDisableHighLight(handInteractInterface.GetObject(), !CanAffordCost(IHandInteractInterface::Execute_GetCost(handInteractInterface.GetObject())));

        IHandInteractInterface::Execute_InteractableEffectStart(handInteractInterface.GetObject());
    }

    currentRightInteractInterface = handInteractInterface;
}

void APlayer_Controller::HandInteractRightOverlapEnd(TScriptInterface<IHandInteractInterface> handInteractInterface)
{
	if (bRightGrabbing && !IHandInteractInterface::Execute_IsOnBoard(handInteractInterface.GetObject()))
	{
		return;
	}

	ReleaseRightInteract(handInteractInterface);
}

void APlayer_Controller::ReleaseRightInteract(TScriptInterface<IHandInteractInterface> handInteractInterface)
{
	if (currentRightInteractInterface != handInteractInterface) return;

	if (currentRightInteractInterface)
	{
		IHandInteractInterface::Execute_SetDisableHighLight(handInteractInterface.GetObject(), false);
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
		IHandInteractInterface::Execute_SetDisableHighLight(handInteractInterface.GetObject(), !CanAffordCost(IHandInteractInterface::Execute_GetCost(handInteractInterface.GetObject())));

        IHandInteractInterface::Execute_InteractableEffectStart(handInteractInterface.GetObject());
    }

    currentLeftInteractInterface = handInteractInterface;
}

void APlayer_Controller::HandInteractLeftOverlapEnd(TScriptInterface<IHandInteractInterface> handInteractInterface)
{
	if (bLeftGrabbing && !IHandInteractInterface::Execute_IsOnBoard(handInteractInterface.GetObject()))
	{
		return;
	}

	ReleaseLeftInteract(handInteractInterface);
}


void APlayer_Controller::ReleaseLeftInteract(TScriptInterface<IHandInteractInterface> handInteractInterface)
{
	if (currentLeftInteractInterface != handInteractInterface) return;

	if (currentLeftInteractInterface)
	{
		IHandInteractInterface::Execute_SetDisableHighLight(handInteractInterface.GetObject(), false);
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

