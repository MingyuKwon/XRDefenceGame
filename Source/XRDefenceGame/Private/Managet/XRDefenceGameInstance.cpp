// Fill out your copyright notice in the Description page of Project Settings.


#include "Managet/XRDefenceGameInstance.h"
#include "Managet/AudioSubsystem.h"


void UXRDefenceGameInstance::Init()
{
	Super::Init();
}

UAudioSubsystem* UXRDefenceGameInstance::GetAudioManagerSubsystem()
{
	return GetSubsystem<UAudioSubsystem>();
}

void UXRDefenceGameInstance::SetPlayerTransformData(FVector PlayerGamePlayLocation, FRotator PlayerGamePlayRotation, FVector GameStartLocatoin)
{
    FVector ReverseLocation = FVector::ZeroVector - PlayerGamePlayLocation;
    FRotator ReverseRotation = PlayerGamePlayRotation * -1;
    FVector RotatedVector = ReverseRotation.RotateVector(GameStartLocatoin + ReverseLocation);

    OffencePlayerGamePlayLocation = RotatedVector;
    OffencePlayerGamePlayRotation = ReverseRotation;

    DefencePlayerGamePlayLocation = FVector(-RotatedVector.X, -RotatedVector.Y, RotatedVector.Z);
    DefencePlayerGamePlayRotation = ReverseRotation + FRotator(0, 180, 0);

}

