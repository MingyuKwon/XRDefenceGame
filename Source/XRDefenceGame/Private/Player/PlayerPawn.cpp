// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/PlayerPawn.h"

APlayerPawn::APlayerPawn()
{
	PrimaryActorTick.bCanEverTick = true;

}

void APlayerPawn::SetPawnTransformForGameStart(FVector MapSpawnLocation, FRotator MapSpawnRotation)
{

    // Location Relative Set
	FVector ReverseLocation = FVector::ZeroVector - MapSpawnLocation;
	FVector ShouldMovetoLocation = GetActorLocation() + ReverseLocation;
	SetActorLocation(ShouldMovetoLocation);

    // Rotation Relative Set
    FRotator ReverseRotation = MapSpawnRotation * -1;
    FVector RotatedVector = ReverseRotation.RotateVector(GetActorLocation());
    SetActorLocation(RotatedVector);

    FRotator NewRotation = GetActorRotation() + ReverseRotation;
    SetActorRotation(NewRotation);
}

