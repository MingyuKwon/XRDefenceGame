// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MainInfoBoard.h"
#include "Mode/XRGamePlayMode.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "Managet/XRDefenceGameInstance.h"

AMainInfoBoard::AMainInfoBoard()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
}


void AMainInfoBoard::BeginPlay()
{
	Super::BeginPlay();

	XRGamePlayMode = Cast<AXRGamePlayMode>(UGameplayStatics::GetGameMode(this));
	if (XRGamePlayMode)
	{
		XRGamePlayMode->OnGameTimerTickEvent.AddDynamic(this, &ThisClass::OnGameTimerShow);
		XRGamePlayMode->OnNexusDamageEvent.AddDynamic(this, &ThisClass::NexusHealthChange);

		XRGamePlayMode->OnGameStart.AddDynamic(this, &ThisClass::OnGameStart);
		XRGamePlayMode->OnGameEnd.AddDynamic(this, &ThisClass::OnGameEnd);

		XRGamePlayMode->OnConnectEvenet.AddDynamic(this, &ThisClass::SetConnectState_Multi);

	}

	XRGameInstace = Cast<UXRDefenceGameInstance>(GetGameInstance());


	WhichPanelToShow_Multi(EGameMatchState::EGMS_FIrstGameWait);

}


void AMainInfoBoard::OnGameStart()
{
	WhichPanelToShow_Multi(EGameMatchState::EGMS_FIrstGamePlaying);

}

void AMainInfoBoard::OnGameEnd()
{
	WhichPanelToShow_Multi(EGameMatchState::EGMS_FIrstGameEnd);
	if (XRGameInstace == nullptr) return;

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("OnGameEnd")));
	}

	if (HasAuthority())
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("SetFinalResultPanel_Multi Call")));
		}
		SetFinalResultPanel_Multi(XRGameInstace->FirstNexusCount, XRGameInstace->FirstNexusHealth, XRGameInstace->FirstTimeLeft, XRGameInstace->SecondNexusCount, XRGameInstace->SecondNexusHealth, XRGameInstace->SecondTimeLeft);
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

	UpdateInGameUI(curerntLeftTime, orangeNexusHealth + blueNexusHealth + purpleNexusHealth, orangeNexusHealth, blueNexusHealth, purpleNexusHealth);
}

void AMainInfoBoard::OnGameTimerShow(float leftSecond)
{
	curerntLeftTime = leftSecond;
	UpdateInGameUI(curerntLeftTime, orangeNexusHealth + blueNexusHealth + purpleNexusHealth, orangeNexusHealth , blueNexusHealth , purpleNexusHealth);

}

void AMainInfoBoard::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void AMainInfoBoard::WhichPanelToShow_Multi_Implementation(EGameMatchState matchState)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("SetConnectState_Multi_Implementation")));
	}
	WhichPanelToShow(matchState);
}

void AMainInfoBoard::SetConnectState_Multi_Implementation(bool offence, bool defence)
{

	SetConnectState(offence, defence);
}

void AMainInfoBoard::UpdateInGameUI_Implementation(float TimeSecond, float TotalHealthAmount, float OrangeHealthAmount, float BlueHealthAmount, float PurpleHealthAmount)
{
	SetUIPurpleHealth(PurpleHealthAmount);
	SetUIOrnageHealth(OrangeHealthAmount);
	SetUBlueHealth(BlueHealthAmount);
	SetUIHealth(TotalHealthAmount);

	SetUITime(TimeSecond);
}

void AMainInfoBoard::SetFinalResultPanel_Multi_Implementation(float FirstNexusCount, float FirstNexusHealth, float FirstTimeLeft, float SecondNexusCount, float SecondNexusHealth, float SecondTimeLeft)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("SetFinalResultPanel_Multi In")));
	}

	SetFinalResultPanel(FirstNexusCount, FirstNexusHealth, FirstTimeLeft, SecondNexusCount, SecondNexusHealth, SecondTimeLeft);
}
