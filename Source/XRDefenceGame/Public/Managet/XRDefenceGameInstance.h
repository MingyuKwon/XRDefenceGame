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

	UFUNCTION(BlueprintCallable)
	void SetPlayerTransformData(FVector PlayerGamePlayLocation, FRotator PlayerGamePlayRotation, FVector GameStartLocatoin);

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Player")
	FVector OffencePlayerGamePlayLocation = FVector(-80.f, 0.f, 40.f);
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Player")
	FRotator OffencePlayerGamePlayRotation = FRotator(0.f, 0.f, 0.f);

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Player")
	FVector DefencePlayerGamePlayLocation = FVector(80.f, 0.f, 40.f);
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Player")
	FRotator DefencePlayerGamePlayRotation = FRotator(0.f, 180.f, 0.f);

	int32 FirstNexusCount = -1;
	int32 FirstNexusHealth = -1;
	int32 FirstTimeLeft = -1;

	int32 SecondNexusCount = -1;
	int32 SecondNexusHealth = -1;
	int32 SecondTimeLeft = -1;
	
	EObjectType ServerObjectType = EObjectType::EOT_Deffence;
	EObjectType ClientObjectType = EObjectType::EOT_Offence;

	EGameMatchState matchState = EGameMatchState::EGMS_None;

};
