// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Offence/MyXR_CharacterOffenceBattle.h"
#include "Character/Deffence/XR_CharacterDeffence.h"
#include "Character/Deffence/MyXR_CharacterDeffenceBuff.h"
#include "Character/WallNexus.h"
#include "Kismet/GameplayStatics.h"

void AMyXR_CharacterOffenceBattle::CharacterActionImpact()
{
	Super::CharacterActionImpact();

	if (TargetCharacter)
	{
		FVector MyLocation = GetActorLocation();
		FVector TargetLocation = TargetCharacter->GetActorLocation();

		FVector Direction = TargetLocation - MyLocation;
		Direction.Z = 0.0f;

		Direction.Normalize();

		FRotator NewRotation = Direction.Rotation();
		SetActorRotation(NewRotation);


		UGameplayStatics::ApplyDamage(TargetCharacter, CharacterProperty.currentDamage, GetController(), this, nullptr);
	}

}

void AMyXR_CharacterOffenceBattle::FindNearbyEnemy(AXR_Character*& outFirstNear, AXR_Character*& outSecondNear)
{
	Super::FindNearbyEnemy(outFirstNear, outSecondNear);

	TArray<AActor*> Nexuses;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AWallNexus::StaticClass(), Nexuses);

	TArray<AActor*> LiveNexuses;

	for (AActor* Actor : Nexuses)
	{
		if (Actor && Actor != this)
		{
			if (IHandInteractInterface::Execute_IsOnBoard(Actor))
			{
				LiveNexuses.Add(Actor);
			}
		}
	}

	LiveNexuses.Sort([this](const AActor& A, const AActor& B)
		{
			return FVector::Dist2D(this->GetActorLocation(), A.GetActorLocation()) < FVector::Dist2D(this->GetActorLocation(), B.GetActorLocation());
		}
	);

	AWallNexus* nearestNexus = (LiveNexuses.Num() > 0) ? Cast<AWallNexus>(LiveNexuses[0]) : nullptr;

	if (nearestNexus == nullptr) return;

	float FromMetoNexusDistance = FVector::Dist2D(this->GetActorLocation(), nearestNexus->GetActorLocation());

	TArray<AActor*> AllCharacters;

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AXR_CharacterDeffence::StaticClass(), AllCharacters);

	TArray<AXR_Character*> NearbyCharacters;

	for (AActor* Actor : AllCharacters)
	{
		if (Actor && Actor != this)
		{
			AXR_Character* xrChar = Cast<AXR_Character>(Actor);

			if (IHandInteractInterface::Execute_IsOnBoard(xrChar) && 
				Cast<AMyXR_CharacterDeffenceBuff>(xrChar) == nullptr && 
				(FromMetoNexusDistance > FVector::Dist2D(xrChar->GetActorLocation(), nearestNexus->GetActorLocation())) )
			{
				NearbyCharacters.Add(xrChar);
			}
		}
	}

	if (NearbyCharacters.Num() == 0) // If No Other Object is closer than Nexus
	{
		outFirstNear = nearestNexus;
		outSecondNear = nullptr;
		return;
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

	if (TargetCharacter)
	{
		FVector MyLocation = GetActorLocation();
		FVector TargetLocation = TargetCharacter->GetActorLocation();

		FVector Direction = TargetLocation - MyLocation;
		Direction.Z = 0.0f;

		Direction.Normalize();

		FRotator NewRotation = Direction.Rotation();
		SetActorRotation(NewRotation);

	}
}

void AMyXR_CharacterOffenceBattle::OtherCharacterSpawnCallBack(FVector spawnLocation)
{
	Super::OtherCharacterSpawnCallBack(spawnLocation);

	float Dist = FVector::Dist2D(GetActorLocation(), spawnLocation);

	SetOffenceTarget();

}

void AMyXR_CharacterOffenceBattle::OnBoardCalledFunctionServer(bool isOnBoard, bool isSpawnedByHand)
{
	Super::OnBoardCalledFunctionServer(isOnBoard, isSpawnedByHand);
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

