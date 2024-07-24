// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Offence/MyXR_CharacterOffenceSpawn.h"
#include "Character/Offence/MyXR_CharacterOffenceBattle.h"

void AMyXR_CharacterOffenceSpawn::SpawnTimerFunction()
{
	GetWorld()->GetTimerManager().SetTimer(SpawnTimerHandle, this, &AMyXR_CharacterOffenceSpawn::SpawnTimerFunction, SpawnDelay, false);

	int32 HaveTpSpawnCount = SpawnCount;

	if (HaveTpSpawnCount > 0)
	{
		SpawnCharacter();
		HaveTpSpawnCount--;
	}

	if (HaveTpSpawnCount > 0)
	{
		GetWorld()->GetTimerManager().SetTimer(Spawn2TimerHandle, this, &AMyXR_CharacterOffenceSpawn::SpawnCharacter, 0.5f, false);
		HaveTpSpawnCount--;
	}

	if (HaveTpSpawnCount > 0)
	{
		GetWorld()->GetTimerManager().SetTimer(Spawn3TimerHandle, this, &AMyXR_CharacterOffenceSpawn::SpawnCharacter, 1.f, false);
		HaveTpSpawnCount--;
	}

}

void AMyXR_CharacterOffenceSpawn::SpawnCharacter()
{
	if (SpawnClass)
	{
		FVector SpawnLocation = GetActorLocation() + GetActorForwardVector() * 1.5f;

		AMyXR_CharacterOffenceBattle* SpawnActor = GetWorld()->SpawnActor<AMyXR_CharacterOffenceBattle>(SpawnClass, SpawnLocation, GetActorRotation());
		SpawnActor->SetOnBoardAuto();
	}
}

void AMyXR_CharacterOffenceSpawn::OnBoardCalledFunctionServer(bool isOnBoard, bool isSpawnedByHand)
{
	Super::OnBoardCalledFunctionServer(isOnBoard, isSpawnedByHand);
}

void AMyXR_CharacterOffenceSpawn::BehaviorAvailableTimerFunction()
{
	Super::BehaviorAvailableTimerFunction();

	if (bOnBoard)
	{
		GetWorld()->GetTimerManager().SetTimer(SpawnTimerHandle, this, &AMyXR_CharacterOffenceSpawn::SpawnTimerFunction, SpawnDelay, false);
	}
}
