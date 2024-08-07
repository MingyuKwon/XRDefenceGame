// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MainInfoBoard.h"
#include "Mode/XRGamePlayMode.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"

AMainInfoBoard::AMainInfoBoard()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
}

void AMainInfoBoard::UpdateUI_Implementation( float TimeSecond, float TotalHealthAmount, float OrangeHealthAmount, float BlueHealthAmount, float PurpleHealthAmount)
{
	SetUIPurpleHealth(PurpleHealthAmount);
	SetUIOrnageHealth(OrangeHealthAmount);
	SetUBlueHealth(BlueHealthAmount);
	SetUIHealth(TotalHealthAmount);

	SetUITime(TimeSecond);
}

void AMainInfoBoard::BeginPlay()
{
	Super::BeginPlay();

	XRGamePlayMode = Cast<AXRGamePlayMode>(UGameplayStatics::GetGameMode(this));
	if (XRGamePlayMode)
	{
		XRGamePlayMode->OnGameTimerTickEvent.AddDynamic(this, &ThisClass::OnGameTimerShow);
		XRGamePlayMode->OnNexusDamageEvent.AddDynamic(this, &ThisClass::NexusHealthChange);
	}

	
}

void AMainInfoBoard::NexusHealthChange(ENexusType nexusType, float currentHealth)
{
	if (nexusType == ENexusType::ENT_NexusPurple)
	{
		purpleNexusHealth = currentHealth;

	}
	else if (nexusType == ENexusType::ENT_NexusOrange)
	{
		orangeNexusHealth = currentHealth;
	}
	else if (nexusType == ENexusType::ENT_NexusBlue)
	{
		blueNexusHealth = currentHealth;

	}

	UpdateUI(curerntLeftTime, orangeNexusHealth + blueNexusHealth + purpleNexusHealth, orangeNexusHealth, blueNexusHealth, purpleNexusHealth);
}

void AMainInfoBoard::OnGameTimerShow(float leftSecond)
{
	curerntLeftTime = leftSecond;
	UpdateUI(curerntLeftTime, orangeNexusHealth + blueNexusHealth + purpleNexusHealth, orangeNexusHealth , blueNexusHealth , purpleNexusHealth);

}

void AMainInfoBoard::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

