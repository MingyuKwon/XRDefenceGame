// Fill out your copyright notice in the Description page of Project Settings.


#include "Managet/AudioSubsystem.h"
#include "Sound/SoundBase.h"
#include "Kismet/GameplayStatics.h"

void UAudioSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

}

void UAudioSubsystem::PlaySound(EGameSoundType soundType, USoundBase* Sound, FVector Location, float VolumeScale)
{
    float Volume = 1.f;

    switch (soundType)
    {
    case EGameSoundType::EGST_SFX :
        Volume = SFXVolume;

    case EGameSoundType::EGST_BGM:
        Volume = BGMVolume;

    case EGameSoundType::EGST_UI:
        Volume = UIVolume;

    default:
        break;
    } 

    Volume *= VolumeScale;

    PlaySoundEffect(Sound, Location, Volume);

    
}

void UAudioSubsystem::PlaySoundEffect(USoundBase* Sound, FVector Location, float Volume)
{
    if (Sound)
    {
        UGameplayStatics::PlaySoundAtLocation(this, Sound, Location, Volume);
    }
}
