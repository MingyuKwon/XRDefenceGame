// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Offence/MyXR_CharacterOffenceBuff.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Kismet/GameplayStatics.h"

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

		PlaySoundViaManager(EGameSoundType::EGST_SFX, SoundBuffRange, GetActorLocation(), 0.4f);

		if (NG)
		{
			NG->SetVariableFloat(FName("Radius"), BuffRadius);
		}
	}

	TArray<AXR_Character*> NearbyCharacters = FindBuffableCharacter();

	if (CharacterType == ECharacterType::ECT_OffenceH)
	{
		for (AXR_Character* xrChar : NearbyCharacters)
		{
			xrChar->Heal(HealAmount);
		}
	}
	else if (CharacterType == ECharacterType::ECT_OffenceB)
	{
		for (AXR_Character* xrChar : NearbyCharacters)
		{
			xrChar->AttackBuff(BuffAmount);

		}
	}
}

TArray<AXR_Character*> AMyXR_CharacterOffenceBuff::FindBuffableCharacter()
{
	TArray<AActor*> AllCharacters;

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AMyXR_CharacterOffenceBattle::StaticClass(), AllCharacters);

	TArray<AXR_Character*> NearbyCharacters;

	for (AActor* Actor : AllCharacters)
	{
		if (Actor && Actor != this)
		{
			AXR_Character* xrChar = Cast<AXR_Character>(Actor);

			double Distance = FVector::Dist2D(this->GetActorLocation(), xrChar->GetActorLocation());

			if (IHandInteractInterface::Execute_IsOnBoard(xrChar) && Distance <= BuffRadius + 3.f)
			{
				NearbyCharacters.Add(xrChar);
			}
		}
	}

	return NearbyCharacters;
}
