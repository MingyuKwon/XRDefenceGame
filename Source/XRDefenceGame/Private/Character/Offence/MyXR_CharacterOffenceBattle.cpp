// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Offence/MyXR_CharacterOffenceBattle.h"
#include "Kismet/GameplayStatics.h"

void AMyXR_CharacterOffenceBattle::CharacterActionImpact()
{
	Super::CharacterActionImpact();

	if (TargetCharacter)
	{
		UGameplayStatics::ApplyDamage(TargetCharacter, CharacterProperty.Damage, GetController(), this, nullptr);
	}

}

void AMyXR_CharacterOffenceBattle::CharacterActionStart()
{
	Super::CharacterActionStart();
}

