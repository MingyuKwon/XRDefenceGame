// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Offence/MyXR_CharacterOffenceBattle.h"
#include "Character/Deffence/XR_CharacterDeffence.h"
#include "Character/Deffence/MyXR_CharacterDeffenceBuff.h"

#include "Kismet/GameplayStatics.h"

void AMyXR_CharacterOffenceBattle::CharacterActionImpact()
{
	Super::CharacterActionImpact();

	if (TargetCharacter)
	{
		UGameplayStatics::ApplyDamage(TargetCharacter, CharacterProperty.currentDamage, GetController(), this, nullptr);
	}

}

void AMyXR_CharacterOffenceBattle::FindNearbyEnemy(AXR_Character*& outFirstNear, AXR_Character*& outSecondNear)
{
	Super::FindNearbyEnemy(outFirstNear, outSecondNear);

	TArray<AActor*> AllCharacters;

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AXR_CharacterDeffence::StaticClass(), AllCharacters);

	TArray<AXR_Character*> NearbyCharacters;

	for (AActor* Actor : AllCharacters)
	{
		if (Actor && Actor != this)
		{
			AXR_Character* xrChar = Cast<AXR_Character>(Actor);

			if (IHandInteractInterface::Execute_IsOnBoard(xrChar) && Cast<AMyXR_CharacterDeffenceBuff>(xrChar) == nullptr)
			{
				NearbyCharacters.Add(xrChar);
			}
		}
	}

	NearbyCharacters.Sort([this](const AXR_Character& A, const AXR_Character& B)
		{
			return FVector::Dist2D(this->GetActorLocation(), A.GetActorLocation()) < FVector::Dist2D(this->GetActorLocation(), B.GetActorLocation());
		}
	);

	outFirstNear = (NearbyCharacters.Num() > 0) ? NearbyCharacters[0] : nullptr;
	outSecondNear = (NearbyCharacters.Num() > 1) ? NearbyCharacters[1] : nullptr;

}

void AMyXR_CharacterOffenceBattle::SetOffenceTarget()
{
	AXR_Character* FirstNear;
	AXR_Character* SecondNear;

	FindNearbyEnemy(FirstNear, SecondNear);

	TargetCharacter = FirstNear;
}

void AMyXR_CharacterOffenceBattle::CharacterActionStart()
{
	Super::CharacterActionStart();
}

void AMyXR_CharacterOffenceBattle::OtherCharacterSpawnCallBack(FVector spawnLocation)
{
	Super::OtherCharacterSpawnCallBack(spawnLocation);

	float Dist = FVector::Dist2D(GetActorLocation(), spawnLocation);

	SetOffenceTarget();

}

void AMyXR_CharacterOffenceBattle::OnBoardCalledFunction(bool isOnBoard, bool isSpawnedByHand)
{
	Super::OnBoardCalledFunction(isOnBoard, isSpawnedByHand);
}

void AMyXR_CharacterOffenceBattle::BehaviorAvailableTimerFunction()
{
	Super::BehaviorAvailableTimerFunction();

	SetOffenceTarget();
}

void AMyXR_CharacterOffenceBattle::TargetDieCallBack(AXR_Character* DieTarget)
{
	Super::TargetDieCallBack(DieTarget);
	SetOffenceTarget();
}

