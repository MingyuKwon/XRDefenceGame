// Fill out your copyright notice in the Description page of Project Settings.


#include "Mode/XRGamePlayMode.h"
#include "Character/XR_Character.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "Player/Player_Controller.h"
#include "Player/PlayerPawn.h"
#include "Managet/XRDefenceGameInstance.h"


void AXRGamePlayMode::TriggerOnMapSpawnPawnMoveEvent(EObjectType objectType, FVector SpawnLocatoin, FRotator SpawnRotation)
{
	OnMapSpawnPawnMoveEvent.Broadcast(objectType, SpawnLocatoin, SpawnRotation);
}

void AXRGamePlayMode::TriggerOnGameStartEvent()
{
	OnGameStart.Broadcast();
	GetWorld()->GetTimerManager().SetTimer(GameTimerHandle, this, &AXRGamePlayMode::GameTimerCallBack, 1.0f, true);
}

void AXRGamePlayMode::TriggerOnGameEndEvent()
{
	OnGameEnd.Broadcast();
	GetWorld()->GetTimerManager().ClearTimer(GameTimerHandle);

}



void AXRGamePlayMode::PostLogin(APlayerController* NewPlayer)
{
    Super::PostLogin(NewPlayer);

	FTimerHandle TimerHandle;
	FTimerDelegate TimerDelegate;

	TimerDelegate.BindUFunction(this, FName("SetPlayerCharacterOnWantedPosition"), NewPlayer);

	GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDelegate, 2.0f, false);
}

void AXRGamePlayMode::SetPlayerCharacterOnWantedPosition(APlayerController* NewPlayer)
{
	APlayerPawn* PlayerPawn = Cast<APlayerPawn>(NewPlayer->GetPawn());
	if (PlayerPawn)
	{
		PlayerPawn->SetPawnTransformForGameStart();
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

	GetWorld()->GetTimerManager().SetTimer(GameStartTimer, [this]() {
		TriggerOnGameStartEvent();
		}, 2.f, false);

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
