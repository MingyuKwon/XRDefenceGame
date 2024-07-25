// Fill out your copyright notice in the Description page of Project Settings.


#include "Battle/CostShowChip.h"

ACostShowChip::ACostShowChip()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
}

void ACostShowChip::SetGoldCostCountMulti_Implementation(float percent)
{
	SetGoldCostCount(percent);
}



