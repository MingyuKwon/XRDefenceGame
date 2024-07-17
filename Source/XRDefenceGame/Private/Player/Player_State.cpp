// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/Player_State.h"
#include "Net/UnrealNetwork.h"

void APlayer_State::SetGold(float NewGold)
{
	Gold = NewGold;
}

float APlayer_State::GetGold() const
{
	return Gold;
}

void APlayer_State::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APlayer_State, Gold);
}