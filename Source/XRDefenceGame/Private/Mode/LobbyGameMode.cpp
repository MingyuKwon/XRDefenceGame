// Fill out your copyright notice in the Description page of Project Settings.


#include "Mode/LobbyGameMode.h"
#include "Managet/XRDefenceGameInstance.h"
#include "XRDefenceEnums.h"

void ALobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	playerCount++;

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("Login Characters")));
		UE_LOG(LogTemp, Display, TEXT("Multi Test Login Characters"));
	}

	if (playerCount >= 2)
	{
		GetWorld()->GetTimerManager().SetTimer(LobbyOutCount, [this]() {
			CountDown();
			}, 1.f, true);

	}
}

void ALobbyGameMode::CountDown()
{
	if (timeCount <= 0) return;

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Count Dwon %d"), timeCount));
		UE_LOG(LogTemp, Display, TEXT("Multi Test Login Characters"));
	}
	timeCount--;
	if (timeCount <= 0)
	{
		UWorld* world = GetWorld();

		if (world)
		{
			UE_LOG(LogTemp, Display, TEXT("Multi Test Move to GameMap"));
			world->ServerTravel(GameMapName);
		}
	}
}

void ALobbyGameMode::BeginPlay()
{
	Super::BeginPlay();

	XRGameInstace = Cast<UXRDefenceGameInstance>(GetGameInstance());
	if (XRGameInstace)
	{
		XRGameInstace->matchState = EGameMatchState::EGMS_FIrstGameWait;

		XRGameInstace->ServerObjectType = EObjectType::EOT_Deffence;
		XRGameInstace->ClientObjectType = EObjectType::EOT_Offence;
	}

}
