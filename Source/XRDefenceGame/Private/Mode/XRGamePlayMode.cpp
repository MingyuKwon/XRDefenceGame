// Fill out your copyright notice in the Description page of Project Settings.


#include "Mode/XRGamePlayMode.h"
#include "Character/XR_Character.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "Player/Player_Controller.h"
#include "Player/PlayerPawn.h"
#include "Managet/XRDefenceGameInstance.h"
#include "MultiplayerSession/Public/MultiplayerSessionsSubsystem.h"
#include <Online/OnlineSessionNames.h>
#include "MultiplayerSessionsSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Managet/XRDefenceGameInstance.h"

void AXRGamePlayMode::InitializeOnlineSubSystem()
{
	UGameInstance* gameInstance = GetGameInstance();
	if (gameInstance)
	{
		MultiplayerSessionsSubsystem = gameInstance->GetSubsystem<UMultiplayerSessionsSubsystem>();
	}

	if (MultiplayerSessionsSubsystem)
	{
		MultiplayerSessionsSubsystem->MultiPlayerOnDestroySessionComplete.AddDynamic(this, &ThisClass::OnDestroySession);
	}
}

void AXRGamePlayMode::DestroyServerSession()
{
	if (MultiplayerSessionsSubsystem)
	{
		MultiplayerSessionsSubsystem->DestroySession();
	}
}

void AXRGamePlayMode::OnDestroySession(bool bwasSuccessFul)
{
	if (bwasSuccessFul)
	{
		UWorld* World = GetWorld();
		if (World)
		{
			for (FConstPlayerControllerIterator It = World->GetPlayerControllerIterator(); It; ++It)
			{
				APlayerController* PlayerController = It->Get();

				if (PlayerController)
				{
					PlayerController->ClientTravel(LobbyMapName, ETravelType::TRAVEL_Absolute);
				}
			}
		}
	}
}


void AXRGamePlayMode::AddActorToMap(int32 ActorNetID, AXR_Character* Actor)
{
	if (Actor && !ActorMap.Contains(ActorNetID))
	{
		ActorMap.Add(ActorNetID, Actor);
	}
}

void AXRGamePlayMode::RemoveActorFromMap(int32 ActorNetID)
{
	ActorMap.Remove(ActorNetID);
}

AXR_Character* AXRGamePlayMode::FindActorInMap(int32 ActorNetID) const
{
	AXR_Character* const* FoundActor = ActorMap.Find(ActorNetID);
	return FoundActor ? *FoundActor : nullptr;
}


void AXRGamePlayMode::TriggerOnMapSpawnPawnMoveEvent(EObjectType objectType, FVector SpawnLocatoin, FRotator SpawnRotation)
{
	OnMapSpawnPawnMoveEvent.Broadcast(objectType, SpawnLocatoin, SpawnRotation);
}

void AXRGamePlayMode::TriggerOnGameStartEvent()
{
	if (isNowFirstGame())
	{
		SetGameMatchState(EGameMatchState::EGMS_FIrstGameStart);

		if (XRGameInstace)
		{
			if (XRGameInstace->ServerObjectType == EObjectType::EOT_Deffence)
			{
				XRGameInstace->bServerFirstDefence = true;
			}
			else
			{
				XRGameInstace->bServerFirstDefence = false;

			}

		}
	}
	else
	{
		SetGameMatchState(EGameMatchState::EGMS_SecondGameStart);
	}

	FTimerHandle TimerHandle;
	FTimerDelegate TimerDelegate;

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Game Start in 5Seconds")));
	}

	GetWorld()->GetTimerManager().SetTimer(TriggerOnGameStartEventTimerHandle, [this]() {

		if (isNowFirstGame())
		{
			SetGameMatchState(EGameMatchState::EGMS_FIrstGamePlaying);
		}
		else
		{
			SetGameMatchState(EGameMatchState::EGMS_SecondGamePlaying);
		}

		OnGameStart.Broadcast();
		GetWorld()->GetTimerManager().SetTimer(GameTimerHandle, this, &AXRGamePlayMode::GameTimerCallBack, 1.0f, true);
		}, 5.0f, false);

}

void AXRGamePlayMode::TriggerOnGameEndEvent()
{
	if (XRGameInstace == nullptr) return;

	GetWorld()->GetTimerManager().ClearTimer(TriggerOnGameStartEventTimerHandle);
	
	if (bSurrenderTriggeredByServer || bSurrenderTriggeredByClient)
	{ // IF Surrender And Game Over
		XRGameInstace->matchState = EGameMatchState::EGMS_SecondGameEnd;

		FTimerHandle DestroySessionHandle;
		GetWorld()->GetTimerManager().SetTimer(DestroySessionHandle, [this]() {
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("                          Surrender And Match Close")));
			}
			DestroyServerSession();
			}, 5.0f, false);
	}
	else
	{ // Game End Normally
		int nexusCount = 3;

		if (orangeNexusHealth <= 0)
		{
			nexusCount--;
		}

		if (blueNexusHealth <= 0)
		{
			nexusCount--;
		}

		if (purpleNexusHealth <= 0)
		{
			nexusCount--;
		}


		if (isNowFirstGame())
		{

			XRGameInstace->matchState = EGameMatchState::EGMS_FIrstGameEnd;

			XRGameInstace->FirstNexusCount = nexusCount;
			XRGameInstace->FirstNexusHealth = orangeNexusHealth + blueNexusHealth + purpleNexusHealth;
			XRGameInstace->FirstTimeLeft = GameTimerSecond;

			MoveToNextGame();

		}
		else
		{
			XRGameInstace->matchState = EGameMatchState::EGMS_SecondGameEnd;

			XRGameInstace->SecondNexusCount = nexusCount;
			XRGameInstace->SecondNexusHealth = orangeNexusHealth + blueNexusHealth + purpleNexusHealth;
			XRGameInstace->SecondTimeLeft = GameTimerSecond;

			FTimerHandle DestroySessionHandle;
			GetWorld()->GetTimerManager().SetTimer(DestroySessionHandle, [this]() {
				if (GEngine)
				{
					GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("                          Session will be closed in 5Seconds")));
				}
				DestroyServerSession();
				}, 5.0f, false);
		}
	}

	OnGameEnd.Broadcast();
	GetWorld()->GetTimerManager().ClearTimer(GameTimerHandle);


}

void AXRGamePlayMode::MoveToNextGame()
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("                          Stage Move in 5Seconds")));
	}

	FTimerHandle MovetoNextStageHandle;
	GetWorld()->GetTimerManager().SetTimer(MovetoNextStageHandle, [this]() {

		// Exchange the Role
		if (XRGameInstace)
		{
			EObjectType temp = XRGameInstace->ServerObjectType;
			XRGameInstace->ServerObjectType = XRGameInstace->ClientObjectType;
			XRGameInstace->ClientObjectType = temp;
		}

		SetGameMatchState(EGameMatchState::EGMS_SecondGameWait);
		UWorld* world = GetWorld();
		if (world)
		{
			world->ServerTravel(NewmapName);
		}		
		
		}, 5.0f, false);

}

void AXRGamePlayMode::TriggerSurrender(bool bServer)
{
	if (bSurrenderTriggeredByServer || bSurrenderTriggeredByClient) return;

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("TriggerSurrender bServer %s"), bServer ? *FString("True") : *FString("False")));
	}

	if (bServer)
	{
		bSurrenderTriggeredByServer = true;

	}
	else
	{
		bSurrenderTriggeredByClient = true;
	}

	TriggerOnGameEndEvent();
}


void AXRGamePlayMode::PostLogin(APlayerController* NewPlayer)
{
    Super::PostLogin(NewPlayer);

	APlayer_Controller* PlayerController = Cast<APlayer_Controller>(NewPlayer);

	if (PlayerController == nullptr) return;

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("            %s      PostLogin"), *NewPlayer->GetName()));
	}

	if (HasAuthority())
	{
		XRGameInstace = (XRGameInstace == nullptr) ? Cast<UXRDefenceGameInstance>(GetGameInstance()) : XRGameInstace;

		if (PlayerController->IsLocalController())
		{
			if (XRGameInstace)
			{
				PlayerController->SetControllerObjectType(XRGameInstace->ServerObjectType);
			}
		}
		else
		{
			if (XRGameInstace)
			{
				PlayerController->SetControllerObjectType(XRGameInstace->ClientObjectType);
			}
		}
	}
}

void AXRGamePlayMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("            %s      Logout"), *Exiting->GetName()));
	}

	if (XRGameInstace == nullptr) return;

	if (XRGameInstace->matchState == EGameMatchState::EGMS_SecondGameEnd || bSurrenderTriggeredByServer || bSurrenderTriggeredByClient)
	{
		ResetGameInsatnceValue();
	}

	// if client logout during gameplay
	if (XRGameInstace->matchState == EGameMatchState::EGMS_FIrstGamePlaying || XRGameInstace->matchState == EGameMatchState::EGMS_SecondGamePlaying)
	{
		ResetGameInsatnceValue();

		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("                          Session will be closed in 5Seconds")));
		}

		// Go to Lobby (Server)
		DestroyServerSession();

	}

}

void AXRGamePlayMode::ResetGameInsatnceValue()
{
	XRGameInstace->bServerFirstDefence = true;


	XRGameInstace->FirstNexusCount = -1;
	XRGameInstace->FirstNexusHealth = -1;
	XRGameInstace->FirstTimeLeft = -1;
	XRGameInstace->SecondNexusCount = -1;
	XRGameInstace->SecondNexusHealth = -1;
	XRGameInstace->SecondTimeLeft = -1;

	XRGameInstace->matchState = EGameMatchState::EGMS_None;

}

void AXRGamePlayMode::SetGameMatchState(EGameMatchState matchState)
{
	if (XRGameInstace)
	{
		XRGameInstace->matchState = matchState;
	}
}

bool AXRGamePlayMode::isNowFirstGame()
{
	EGameMatchState currentState = XRGameInstace->matchState;

	if (currentState >= EGameMatchState::EGMS_SecondGameWait)
	{
		return false;
	}
	else if (currentState >= EGameMatchState::EGMS_FIrstGameWait)
	{
		return true;
	}

	return false;

}



void AXRGamePlayMode::PlayerPositionSetReady()
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Multi Test PlayerPositionSetReady")));
	}

	currentconnectPlayer++;

	if (currentconnectPlayer == 1)
	{
		TriggerConnectUIUpdate(XRGameInstace->ServerObjectType);
	}
	else if (currentconnectPlayer == 2)
	{
		TriggerConnectUIUpdate(XRGameInstace->ClientObjectType);
	}
	ShouldGameStart();
}

void AXRGamePlayMode::ShouldGameStart()
{
	if (currentconnectPlayer >= 2)
	{
		TriggerOnGameStartEvent();
	}
}

void AXRGamePlayMode::TriggerOnNexusDamageEventEvent(ENexusType nexusType, float currentHealth)
{
	OnNexusDamageEvent.Broadcast(nexusType, currentHealth);

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

	if (purpleNexusHealth + orangeNexusHealth + blueNexusHealth <= 0)
	{
		TriggerOnGameEndEvent();
	}
}

void AXRGamePlayMode::AddGoldCount(EObjectType objectType)
{
	if (objectType == EObjectType::EOT_Deffence)
	{
		DefenceGoldCount++;
	}
	else if (objectType == EObjectType::EOT_Offence)
	{
		OffenceGoldCount++;
	}
}

void AXRGamePlayMode::TriggerConnectUIUpdate(EObjectType objectType)
{
	if (objectType == EObjectType::EOT_Offence)
	{
		bOffenceConnect = true;
	}
	else if (objectType == EObjectType::EOT_Deffence)
	{
		bDefenceConnect = true;
	}

	OnConnectEvenet.Broadcast(bOffenceConnect, bDefenceConnect);
}

void AXRGamePlayMode::GameTimerCallBack()
{
	if (GameTimerSecond <= 0) return;
		 
	GameTimerSecond = GameTimerSecond - 1.f;
	OnGameTimerTickEvent.Broadcast(GameTimerSecond);

	if (GameTimerSecond <= 0)
	{
		TriggerOnGameEndEvent();
	}
}



void AXRGamePlayMode::BeginPlay()
{
	Super::BeginPlay();

	XRGameInstace = Cast<UXRDefenceGameInstance>(GetGameInstance());

	InitializeOnlineSubSystem();

}


void AXRGamePlayMode::TriggerOnObjectGrabEvent(bool isGrab, EObjectType objectType)
{
	OnObjectGrabEvent.Broadcast(isGrab, objectType);

}

void AXRGamePlayMode::SpawnActorForUpgrade_Implementation(ECharacterType characterType, FVector position, FRotator rotation, FCharacterValueTransmitForm form)
{
}


AXR_Character* AXRGamePlayMode::PoolSpawnActor_Implementation(ECharacterType characterType)
{
	return nullptr;
}
