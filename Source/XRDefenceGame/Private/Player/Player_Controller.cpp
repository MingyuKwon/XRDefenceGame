// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/Player_Controller.h"
#include "Player/PlayerPawn.h"
#include "Player/Player_State.h"
#include "Kismet/GameplayStatics.h"
#include "Mode/XRGamePlayMode.h"
#include "Net/UnrealNetwork.h"
#include "Character/XR_Character.h"

APlayer_Controller::APlayer_Controller()
{
	bReplicates = true;
	FString name = GetName();
}

void APlayer_Controller::Tick(float DeltaTime)
{
	if (!IsLocalPlayerController()) return;

	if (!GetPlayerPawn()) return;


	if (!HasAuthority())
	{
		Server_InteractableEffectStart(-1);
	}

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

void APlayer_Controller::UpdateUserHandUI_Implementation()
{
	if (!GetPlayerPawn()) return;
	if (!GetPlayer_State()) return;

	playerPawn->UpdateUserLeftHandUI(playerState->GetGold(), playerState->GetMaxGold(), 
		curerntLeftTime, 
		purpleNexusHealth + orangeNexusHealth + blueNexusHealth, 
		orangeNexusHealth, blueNexusHealth, purpleNexusHealth,
		1 - (float)GestureCoolTime / (float)GestureCoolTimeUnit
		);
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

	if (HasAuthority())
	{
		if (IsLocalController())
		{
			SetControllerObjectType(EObjectType::EOT_Deffence);
		}
		else
		{
			SetControllerObjectType(EObjectType::EOT_Offence);
		}
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

	FString name = GetName();

	if (playerPawn == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s  APlayer_Controller::GetPlayerPawn : playerPawn == nullptr"), *name);
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

void APlayer_Controller::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APlayer_Controller, controllerObjectType);

}



void APlayer_Controller::UpdateCurrentLeftPose(Pose inputPose)
{
	if (!IsLocalController()) return;
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
	if (!IsLocalController()) return;

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
	if (!IsLocalController()) return;

	if (currentRightGesture == EGesture::None) return;

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(2, 0.1f, FColor::Yellow, FString::Printf(TEXT("                                                                 Multi Test ShouldRightGestureRelease")));
	}


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
	if (!IsLocalController()) return;

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


/// /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void APlayer_Controller::HandInteractRightOverlapStart(TScriptInterface<IHandInteractInterface> handInteractInterface)
{
	if (!IsLocalController()) return;

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
		if (HasAuthority())
		{
			InteractableEffectStart(handInteractInterface->GetNetId_Implementation());
		}
		else
		{
			Server_InteractableEffectStart(handInteractInterface->GetNetId_Implementation());

		}
    }

    currentRightInteractInterface = handInteractInterface;
}

void APlayer_Controller::HandInteractRightOverlapEnd(TScriptInterface<IHandInteractInterface> handInteractInterface)
{
	if (!IsLocalController()) return;

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(5, 0.1f, FColor::Yellow, FString::Printf(TEXT("                                                                 Multi Test HandInteractRightOverlapEnd")));
	}

	if (bRightGrabbing && !IHandInteractInterface::Execute_IsOnBoard(handInteractInterface.GetObject()))
	{
		return;
	}

	ReleaseRightInteract(handInteractInterface);
}

void APlayer_Controller::ReleaseRightInteract(TScriptInterface<IHandInteractInterface> handInteractInterface)
{
	if (!IsLocalController()) return;

	if (currentRightInteractInterface != handInteractInterface) return;

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(6, 0.1f, FColor::Yellow, FString::Printf(TEXT("                                                                 Multi Test ReleaseRightInteract")));
	}

	if (currentRightInteractInterface)
	{
		IHandInteractInterface::Execute_SetDisableHighLight(handInteractInterface.GetObject(), false);
		IHandInteractInterface::Execute_InteractableEffectEnd(currentRightInteractInterface.GetObject());
	}

	currentRightInteractInterface = nullptr;
}


void APlayer_Controller::HandInteractLeftOverlapStart(TScriptInterface<IHandInteractInterface> handInteractInterface)
{
	if (!IsLocalController()) return;

	if (bLeftGrabbing) return;

	if (currentLeftInteractInterface == handInteractInterface) return;
	if (currentRightInteractInterface == handInteractInterface) return;

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(7, 0.1f, FColor::Yellow, FString::Printf(TEXT("                                                                 Multi Test HandInteractLeftOverlapStart")));
	}

    if (currentLeftInteractInterface)
    {
        IHandInteractInterface::Execute_InteractableEffectEnd(currentLeftInteractInterface.GetObject());
    }

    if (handInteractInterface)
    {
		IHandInteractInterface::Execute_SetDisableHighLight(handInteractInterface.GetObject(), !CanAffordCost(IHandInteractInterface::Execute_GetCost(handInteractInterface.GetObject())));
		if (HasAuthority())
		{
			InteractableEffectStart(handInteractInterface->GetNetId_Implementation());
		}
		else
		{
			Server_InteractableEffectStart(handInteractInterface->GetNetId_Implementation());

		}
	}

    currentLeftInteractInterface = handInteractInterface;
}

void APlayer_Controller::HandInteractLeftOverlapEnd(TScriptInterface<IHandInteractInterface> handInteractInterface)
{
	if (!IsLocalController()) return;

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(8, 0.1f, FColor::Yellow, FString::Printf(TEXT("                                                                 Multi Test HandInteractLeftOverlapEnd")));

	}
	if (bLeftGrabbing && !IHandInteractInterface::Execute_IsOnBoard(handInteractInterface.GetObject()))
	{
		return;
	}

	ReleaseLeftInteract(handInteractInterface);
}


void APlayer_Controller::ReleaseLeftInteract(TScriptInterface<IHandInteractInterface> handInteractInterface)
{
	if (!IsLocalController()) return;

	if (currentLeftInteractInterface != handInteractInterface) return;

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(9, 0.1f, FColor::Yellow, FString::Printf(TEXT("                                                                 Multi Test ReleaseLeftInteract")));
	}

	if (currentLeftInteractInterface)
	{
		IHandInteractInterface::Execute_SetDisableHighLight(handInteractInterface.GetObject(), false);
		IHandInteractInterface::Execute_InteractableEffectEnd(currentLeftInteractInterface.GetObject());
	}

	currentLeftInteractInterface = nullptr;
}


void APlayer_Controller::LeftGrabStart()
{
	if (!IsLocalController()) return;

	if (bLeftGrabbing) return;

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(10, 0.1f, FColor::Yellow, FString::Printf(TEXT("                                                                 Multi Test LeftGrabStart")));
	}

	if (IsLeftGrabable())
	{
		IHandInteractInterface::Execute_GrabStart(currentLeftInteractInterface.GetObject());
	}

	bLeftGrabbing = true;

}

void APlayer_Controller::LeftGrabEnd()
{
	if (!IsLocalController()) return;

	if (!bLeftGrabbing) return;

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(11, 0.1f, FColor::Yellow, FString::Printf(TEXT("                                                                 Multi Test LeftGrabEnd")));
	}

	if (IsLeftGrabable())
	{
		IHandInteractInterface::Execute_GrabEnd(currentLeftInteractInterface.GetObject());
		ReleaseLeftInteract(currentLeftInteractInterface);
	}

	bLeftGrabbing = false;

}  

void APlayer_Controller::RightGrabStart()
{
	if (!IsLocalController()) return;

	if (bRightGrabbing) return;

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(12, 0.1f, FColor::Yellow, FString::Printf(TEXT("                                                                 Multi Test RightGrabStart")));
	}

	if (IsRightGrabable())
	{
		IHandInteractInterface::Execute_GrabStart(currentRightInteractInterface.GetObject());
	}

	bRightGrabbing = true;

}

void APlayer_Controller::RightGrabEnd()
{
	if (!IsLocalController()) return;

	if (!bRightGrabbing) return;

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(13, 0.1f, FColor::Yellow, FString::Printf(TEXT("                                                                 Multi Test RightGrabEnd")));
	}

	if (IsRightGrabable())
	{
		IHandInteractInterface::Execute_GrabEnd(currentRightInteractInterface.GetObject());
		ReleaseRightInteract(currentRightInteractInterface);


	}

	bRightGrabbing = false;

}

void APlayer_Controller::InteractableEffectStart(int32 NetWorkID)
{
	if (XRGamePlayMode)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(101, 1.f, FColor::Yellow, FString::Printf(TEXT("                                                                 Multi Test Server_InteractableEffectStart_Implementation 2")));
		}

		AXR_Character* Target_inServer = XRGamePlayMode->FindActorInMap(NetWorkID);
		if (Target_inServer)
		{
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(102, 1.f, FColor::Yellow, FString::Printf(TEXT("                                                                 Multi Test Server_InteractableEffectStart_Implementation 3")));
			}
			Target_inServer->InteractableEffectStart_Implementation();
		}
	}
}

void APlayer_Controller::Server_InteractableEffectStart_Implementation(int32 NetWorkID)
{
	if (!HasAuthority()) return;

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(100, 1.f, FColor::Yellow, FString::Printf(TEXT("                                                                 Multi Test Server_InteractableEffectStart_Implementation 1")));
	}

	InteractableEffectStart(NetWorkID);
}

