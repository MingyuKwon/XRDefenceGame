// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "XRDefenceEnums.h"
#include "XRDefenceGameInstance.generated.h"

class USoundBase;
/**
 * 
 */
UCLASS()
class XRDEFENCEGAME_API UXRDefenceGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	virtual void Init() override;

	class UAudioSubsystem* GetAudioManagerSubsystem();

};
