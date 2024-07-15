// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "XRDefenceEnums.h"

#include "AudioSubsystem.generated.h"

/**
 * 
 */

class USoundBase;

UCLASS()
class XRDEFENCEGAME_API UAudioSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	void PlaySound(EGameSoundType soundType, USoundBase* Sound, FVector Location, float VolumeScale);


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound Parameter")
	float SFXVolume = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound Parameter")
	float BGMVolume = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound Parameter")
	float UIVolume = 1.f;

private:
	void PlaySoundEffect(USoundBase* Sound, FVector Location, float Volume);
};
