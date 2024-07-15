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

