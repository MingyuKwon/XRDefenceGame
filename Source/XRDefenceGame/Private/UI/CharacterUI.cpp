// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/CharacterUI.h"

// Sets default values
ACharacterUI::ACharacterUI()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
}

void ACharacterUI::SetHealthPercentMulticast_Implementation(float percent)
{
	SetHealthPercent(percent);
}


void ACharacterUI::SetDamageCountMulticast_Implementation(int32 count)
{
	SetDamageCount(count);
}

void ACharacterUI::SetUtilCountMulticast_Implementation(int32 count)
{
	SetUtilCount(count);
}

void ACharacterUI::SetDamgeUtilVisibleMulticast_Implementation(bool flag)
{
	SetDamgeUtilVisible(flag);
}


