// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/PlayerPawn.h"

APlayerPawn::APlayerPawn()
{
	PrimaryActorTick.bCanEverTick = true;

}

void APlayerPawn::SetPawnTransformForGameStart(FVector MapSpawnLocation, FRotator MapSpawnRotation)
{
	FVector ReverseLocation = FVector::ZeroVector - MapSpawnLocation;

	SetActorLocation(GetActorLocation() + ReverseLocation);

}
