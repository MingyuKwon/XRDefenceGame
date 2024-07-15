// Fill out your copyright notice in the Description page of Project Settings.


#include "Managet/XRDefenceGameInstance.h"
#include "Managet/AudioSubsystem.h"


void UXRDefenceGameInstance::Init()
{
	Super::Init();
}

UAudioSubsystem* UXRDefenceGameInstance::GetAudioManagerSubsystem()
{
	UAudioSubsystem* SSS = GetSubsystem<UAudioSubsystem>();

	if (!bAudioManagerInitailize)
	{
		SSS->SFXTypeMap = SFXTypeMap;
		SSS->BGMTypeMap = BGMTypeMap;
		SSS->UITypeMap = UITypeMap;
	}

	bAudioManagerInitailize = true;

	return SSS;
}
