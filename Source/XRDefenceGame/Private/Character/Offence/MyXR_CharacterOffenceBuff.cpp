// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Offence/MyXR_CharacterOffenceBuff.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"

void AMyXR_CharacterOffenceBuff::OnBoardCalledFunction(bool isOnBoard, bool isSpawnedByHand)
{
	Super::OnBoardCalledFunction(isOnBoard, isSpawnedByHand);
}

void AMyXR_CharacterOffenceBuff::BehaviorAvailableTimerFunction()
{
	Super::BehaviorAvailableTimerFunction();

	if (bOnBoard)
	{
		ApplyBuffInRange();
		GetWorld()->GetTimerManager().SetTimer(BuffTimerHandle, this, &AMyXR_CharacterOffenceBuff::BuffTimerFunction, BuffDelay, false);
	}
}

void AMyXR_CharacterOffenceBuff::BuffTimerFunction()
{
	ApplyBuffInRange();
	GetWorld()->GetTimerManager().SetTimer(BuffTimerHandle, this, &AMyXR_CharacterOffenceBuff::BuffTimerFunction, BuffDelay, false);
}

void AMyXR_CharacterOffenceBuff::ApplyBuffInRange()
{
	if (BuffRangeNiagara)
	{
		UNiagaraComponent* NG = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), BuffRangeNiagara, GetRingPosition(), FRotator::ZeroRotator, FVector(1.0f), true);

		if (NG)
		{
			NG->SetVariableFloat(FName("Radius"), BuffRadius);
		}
	}

	if (CharacterType == ECharacterType::ECT_OffenceH)
	{
		
	}
	else if (CharacterType == ECharacterType::ECT_OffenceB)
	{

	}
}
