// Fill out your copyright notice in the Description page of Project Settings.


#include "Mode/MapLocationSetGameMode.h"
#include "MultiplayerSession/Public/MultiplayerSessionsSubsystem.h"
#include <Online/OnlineSessionNames.h>
#include "MultiplayerSessionsSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Interfaces/OnlineSessionInterface.h"


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

void AMapLocationSetGameMode::MoveToLobby()
{
    if (bOpenLevelServer)
    {
        CreateSessionThroughSubSystem();
    }
    else
    {
        JoinSessionThroughSubSystem();
    }
}

void AMapLocationSetGameMode::BeginPlay()
{
    Super::BeginPlay();

    InitializeOnlineSubSystem();
}

void AMapLocationSetGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

}

void AMapLocationSetGameMode::InitializeOnlineSubSystem()
{
    UGameInstance* gameInstance = GetGameInstance();
    if (gameInstance)
    {
        MultiplayerSessionsSubsystem = gameInstance->GetSubsystem<UMultiplayerSessionsSubsystem>();
    }

    if (MultiplayerSessionsSubsystem)
    {
        MultiplayerSessionsSubsystem->ResetSessionInterface();

        MultiplayerSessionsSubsystem->MultiPlayerOnCreateSessionComplete.AddDynamic(this, &ThisClass::OnCreateSession);
        MultiplayerSessionsSubsystem->MultiPlayerOnFindSessionComplete.AddUObject(this, &ThisClass::OnFindSession);
        MultiplayerSessionsSubsystem->MultiPlayerOnJoinSessionComplete.AddUObject(this, &ThisClass::OnJoinSession);
        MultiplayerSessionsSubsystem->MultiPlayerOnDestroySessionComplete.AddDynamic(this, &ThisClass::OnDestroySession);
        MultiplayerSessionsSubsystem->MultiPlayerOnStartSessionComplete.AddDynamic(this, &ThisClass::OnStartSession);

    }
}

void AMapLocationSetGameMode::CreateSessionThroughSubSystem()
{
    if (MultiplayerSessionsSubsystem)
    {
        MultiplayerSessionsSubsystem->CreateSession(2, FString("FreeForAll"));
    }
}

void AMapLocationSetGameMode::JoinSessionThroughSubSystem()
{
    if (MultiplayerSessionsSubsystem)
    {
        MultiplayerSessionsSubsystem->FindSession(1000);

    }
}

void AMapLocationSetGameMode::OnCreateSession(bool bwasSuccessFul)
{
    if (!bwasSuccessFul) return;

    UWorld* world = GetWorld();

    if (world)
    {
        UE_LOG(LogTemp, Display, TEXT("Multi Test Move to Lobby"));
        world->ServerTravel(LobbyMapName);
    }

    
}

void AMapLocationSetGameMode::OnFindSession(const TArray<FOnlineSessionSearchResult>& SessionResult, bool bWasSuccessFul)
{
    if (MultiplayerSessionsSubsystem == nullptr) return;

    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("OnFindSession called. Success: %s, Number of sessions: %d"), bWasSuccessFul ? TEXT("true") : TEXT("false"), SessionResult.Num()));
    }

    if (bWasSuccessFul)
    {
        for (auto Result : SessionResult)
        {
            FString SettingString;
            Result.Session.SessionSettings.Get(FName("MatchType"), SettingString);

            if (GEngine)
            {
                GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("Found session with MatchType: %s"), *SettingString));
            }

            if (SettingString == FString("FreeForAll"))
            {
                MultiplayerSessionsSubsystem->JoinSession(Result);

                if (GEngine)
                {
                    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString("Joining FreeForAll session"));
                }

                return;
            }
        }
    }
    else
    {
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString("Failed to find any sessions"));
        }
    }
}

void AMapLocationSetGameMode::OnJoinSession(EOnJoinSessionCompleteResult::Type Result)
{
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("OnJoinSession called with result: %d"), static_cast<int32>(Result)));
    }

    IOnlineSubsystem* OnlineSubSystem = IOnlineSubsystem::Get();
    if (OnlineSubSystem)
    {
        IOnlineSessionPtr SessionInterface = OnlineSubSystem->GetSessionInterface();
        if (SessionInterface.IsValid())
        {
            FString Address;
            if (SessionInterface->GetResolvedConnectString(NAME_GameSession, Address))
            {
                if (GEngine)
                {
                    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Resolved connect string: %s"), *Address));
                }

                APlayerController* playerController = GetGameInstance()->GetFirstLocalPlayerController();
                if (playerController)
                {
                    playerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
                    if (GEngine)
                    {
                        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString("ClientTravel initiated successfully"));
                    }
                }
                else
                {
                    if (GEngine)
                    {
                        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString("OnJoinSession failed: playerController is null"));
                    }
                }
            }

        }

    }

}

void AMapLocationSetGameMode::OnDestroySession(bool bwasSuccessFul)
{
}

void AMapLocationSetGameMode::OnStartSession(bool bwasSuccessFul)
{
}
