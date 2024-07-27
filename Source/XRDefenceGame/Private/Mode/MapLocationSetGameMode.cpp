// Fill out your copyright notice in the Description page of Project Settings.


#include "Mode/MapLocationSetGameMode.h"

void AMapLocationSetGameMode::TriggerOnMapRotateEvent(float RotateAmount)
{
	OnMapRotateEvent.Broadcast(RotateAmount);
}

void AMapLocationSetGameMode::TriggerOnMapLocationEvent(FVector SpawnLocation)
{
	OnMapLocationEvent.Broadcast(SpawnLocation);
}

void AMapLocationSetGameMode::TriggerOnMapSpawnEvent()
{
	OnMapSpawnEvent.Broadcast();
}

void AMapLocationSetGameMode::BeginPlay()
{
    Super::BeginPlay();
}

void AMapLocationSetGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	PostTravelSetPlayerLocation();
}

void AMapLocationSetGameMode::PostTravelSetPlayerLocation()
{
    for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
    {
        APlayerController* PlayerController = Iterator->Get();
        if (PlayerController)
        {
            UE_LOG(LogTemp, Warning, TEXT("Debug PostTravelSetPlayerLocation %s"), *PlayerController->GetName());

            if (PlayerController->HasAuthority())
            {
                APawn* PlayerPawn = PlayerController->GetPawn();
                if (PlayerPawn)
                {
                    UE_LOG(LogTemp, Warning, TEXT("Debug PostTravelSetPlayerLocation2 %s"), *PlayerController->GetName());

                    if (PlayerController->IsLocalController())
                    {
                        UE_LOG(LogTemp, Warning, TEXT("Debug PostTravelSetPlayerLocation3 %s"), *PlayerController->GetName());

                        PlayerPawn->SetActorLocation(FVector(80.0f, 0.0f, 40.0f));
                        PlayerPawn->SetActorRotation(FRotator(0, 180, 0));
                    }
                    else
                    {
                        UE_LOG(LogTemp, Warning, TEXT("Debug PostTravelSetPlayerLocation4 %s"), *PlayerController->GetName());

                        PlayerPawn->SetActorLocation(FVector(-80.0f, 0.0f, 40.0f));
                        PlayerPawn->SetActorRotation(FRotator(0, 0, 0));

                    }
                }
            }
        }
    }
}