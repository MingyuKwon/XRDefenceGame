// Fill out your copyright notice in the Description page of Project Settings.


#include "Managet/AudioSubsystem.h"
#include "Sound/SoundBase.h"
#include "Kismet/GameplayStatics.h"

void UAudioSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

}

void UAudioSubsystem::PlaySound(EGameSoundType soundType, EDetailSoundType sfxType, FVector Location, float VolumeScale)
{
    TMap<EDetailSoundType, USoundBase*> TargetMap;
    float Volume = 1.f;

    switch (soundType)
    {
    case EGameSoundType::EGST_SFX :
        TargetMap = SFXTypeMap;
        Volume = SFXVolume;

    case EGameSoundType::EGST_BGM:
        TargetMap = BGMTypeMap;
        Volume = BGMVolume;

    case EGameSoundType::EGST_UI:
        TargetMap = UITypeMap;
        Volume = UIVolume;

    default:
        break;
    } 

    Volume *= VolumeScale;

    UE_LOG(LogTemp, Warning, TEXT("SFXTypeMap Count %f"), SFXTypeMap.Num());
    UE_LOG(LogTemp, Warning, TEXT("BGMTypeMap Count %f"), BGMTypeMap.Num());
    UE_LOG(LogTemp, Warning, TEXT("UITypeMap Count %f"), UITypeMap.Num());

    if (TargetMap.Num() <= 0) return;

    if (TargetMap.Contains(sfxType))
    {
        PlaySoundEffect(*(TargetMap.Find(sfxType)), Location, Volume);
    }
    
}

void UAudioSubsystem::PlaySoundEffect(USoundBase* Sound, FVector Location, float Volume)
{
    if (Sound)
    {
        UE_LOG(LogTemp, Warning, TEXT("PlaySoundViaManager"));

        UGameplayStatics::PlaySoundAtLocation(this, Sound, Location, Volume);
    }
}
