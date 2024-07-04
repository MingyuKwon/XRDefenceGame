// Fill out your copyright notice in the Description page of Project Settings.


#include "Mode/XRGamePlayMode.h"
#include "Character/XR_Character.h"

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
