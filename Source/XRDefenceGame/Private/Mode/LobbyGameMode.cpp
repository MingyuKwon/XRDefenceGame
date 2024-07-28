// Fill out your copyright notice in the Description page of Project Settings.


#include "Mode/LobbyGameMode.h"

void ALobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	playerCount++;

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("Login Characters")));
		UE_LOG(LogTemp, Display, TEXT("Multi Test Login Characters"));

	}

	if (playerCount >= 2)
	{
		UWorld* world = GetWorld();

		if (world)
		{
			UE_LOG(LogTemp, Display, TEXT("Multi Test Move to GameMap"));
			world->ServerTravel(GameMapName);
		}

	}
}
