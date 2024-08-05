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

void AXRGamePlayMode::InitializeOnlineSubSystem()
{
	UGameInstance* gameInstance = GetGameInstance();
	if (gameInstance)
	{
		MultiplayerSessionsSubsystem = gameInstance->GetSubsystem<UMultiplayerSessionsSubsystem>();
	}
}

void AXRGamePlayMode::DestroyServerSession()
{
	if (MultiplayerSessionsSubsystem)
	{
		MultiplayerSessionsSubsystem->DestroySession();
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
	FTimerHandle TimerHandle;
	FTimerDelegate TimerDelegate;

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Game Start in 5Seconds")));
	}

	FTimerHandle TriggerOnGameStartEventTimerHandle;

	GetWorld()->GetTimerManager().SetTimer(TriggerOnGameStartEventTimerHandle, [this]() {

		OnGameStart.Broadcast();
		GetWorld()->GetTimerManager().SetTimer(GameTimerHandle, this, &AXRGamePlayMode::GameTimerCallBack, 1.0f, true);
		}, 5.0f, false);

}

void AXRGamePlayMode::TriggerOnGameEndEvent()
{
	OnGameEnd.Broadcast();
	GetWorld()->GetTimerManager().ClearTimer(GameTimerHandle);
}



void AXRGamePlayMode::PostLogin(APlayerController* NewPlayer)
{
    Super::PostLogin(NewPlayer);

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor:: Red, FString::Printf(TEXT("Multi Test XRGameMode PostLogin Test")));
		UE_LOG(LogTemp, Display, TEXT("Multi Test XRGameMode PostLogin Test"));
	}
}

void AXRGamePlayMode::PlayerPositionSetReady()
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Multi Test PlayerPositionSetReady")));
		UE_LOG(LogTemp, Display, TEXT("Multi Test XRGameMode PostLogin Test"));
	}

	currentconnectPlayer++;
	ShouldGameStart();
}

void AXRGamePlayMode::ShouldGameStart()
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Multi Test ShouldGameStart")));
		UE_LOG(LogTemp, Display, TEXT("Multi Test XRGameMode PostLogin Test"));
	}

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


	UE_LOG(LogTemp, Display, TEXT("OffenceGoldCount   : %d         DefenceGoldCount : %d  "), OffenceGoldCount,  DefenceGoldCount);

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
