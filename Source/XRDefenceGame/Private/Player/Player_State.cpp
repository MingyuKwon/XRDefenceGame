// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/Player_State.h"
#include "Net/UnrealNetwork.h"

void APlayer_State::SetGold(float NewGold)
{
	CurrentGold = FMath::Clamp(NewGold, 0.f, MaxGold);

}

float APlayer_State::GetGold() const
{
	return CurrentGold;
}

void APlayer_State::UpgradeMaxGold(bool bIncrease)
{
	if (bIncrease)
	{
		MaxGold += MaxGoldUpgradeUnit;
	}
	else
	{
		MaxGold -= MaxGoldUpgradeUnit;
	}

	CurrentGold = FMath::Clamp(CurrentGold, -100.f, MaxGold);
}

float APlayer_State::GetMaxGold() const
{
	return MaxGold;
}


void APlayer_State::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APlayer_State, CurrentGold);
}