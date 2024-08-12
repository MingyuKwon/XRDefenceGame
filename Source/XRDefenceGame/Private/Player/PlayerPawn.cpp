// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/PlayerPawn.h"
#include "Kismet/GameplayStatics.h"
#include "Character/Offence/MyXR_CharacterOffenceBattle.h"
#include "Character/Deffence/MyXR_CharacterDeffenceBattle.h"
#include "Interface/HandInteractInterface.h"
#include "Managet/XRDefenceGameInstance.h"
#include "Mode/XRGamePlayMode.h"
#include "Kismet/GameplayStatics.h"
#include "Player/Player_Controller.h"

APlayerPawn::APlayerPawn()
{
	PrimaryActorTick.bCanEverTick = true;
    bReplicates = true;

}

void APlayerPawn::SetPawnTransformForGameStart()
{
    if (GetController() == nullptr) return;

    if (!(GetController()->IsLocalController())) return;

    UXRDefenceGameInstance* GI = Cast<UXRDefenceGameInstance>(GetGameInstance());
    if (GI == nullptr) return;

    PlayerController = Cast<APlayer_Controller>(GetController());

    if (PlayerController)
    {
        if (PlayerController->controllerObjectType == EObjectType::EOT_Offence)
        {
            SetActorLocation(GI->OffencePlayerGamePlayLocation);
            SetActorRotation(GI->OffencePlayerGamePlayRotation);

        }
        else if (PlayerController->controllerObjectType == EObjectType::EOT_Deffence)
        {
            SetActorLocation(GI->DefencePlayerGamePlayLocation);
            SetActorRotation(GI->DefencePlayerGamePlayRotation);

        }
    }

}

TArray<AXR_Character*> APlayerPawn::GetRangeCharacters(FVector impactPoint, float radius, EObjectType objectype)
{
    TArray<AActor*> AllCharacters;
    TArray<AXR_Character*> NearbyCharacters;

    if (objectype == EObjectType::EOT_Offence)
    {
        UGameplayStatics::GetAllActorsOfClass(GetWorld(), AMyXR_CharacterDeffenceBattle::StaticClass(), AllCharacters);

    }
    else if (objectype == EObjectType::EOT_Deffence)
    {
        UGameplayStatics::GetAllActorsOfClass(GetWorld(), AMyXR_CharacterOffenceBattle::StaticClass(), AllCharacters);
    }
    else
    {
        return NearbyCharacters;
    }


    for (AActor* Actor : AllCharacters)
    {
        if (Actor && Actor != this)
        {
            AXR_Character* xrChar = Cast<AXR_Character>(Actor);

            if (xrChar)
            {
                if (IHandInteractInterface::Execute_IsOnBoard(xrChar))
                {
                    float Distance = FVector::Dist2D(impactPoint , Actor->GetActorLocation());

                    if (Distance <= radius)
                    {
                        NearbyCharacters.Add(xrChar);
                    }
                }
            }
        }
    }

	return NearbyCharacters;

}

void APlayerPawn::UpdateUserLeftHandUI_Implementation(float GoldAmount, float MaxGoldAmount, float GoldMineCount, float GesturePercent)
{
    SetUIGoldAmount(GoldAmount, MaxGoldAmount, GoldMineCount - 1);
    SetUIGestureCoolTime(GesturePercent);

}

void APlayerPawn::BeginPlay()
{
    Super::BeginPlay();

    if (bDefaultPawn)
    {
        return;
    }

    PlayerController = Cast<APlayer_Controller>(GetController());
    XRGamePlayMode = Cast<AXRGamePlayMode>(UGameplayStatics::GetGameMode(this));

    if (GetController() && GetController()->IsLocalPlayerController())
    {
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("       PAWN     %s      BeginPlay"), *GetName()));
        }

        if (HasAuthority())
        {
            SetPawnTransformForGameStart();
            ServerGameModeCallPositionReady();
        }
        else
        {
            FTimerHandle SetPositionHandle;
            GetWorld()->GetTimerManager().SetTimer(SetPositionHandle, [this]() {

                SetPawnTransformForGameStart();
                ServerGameModeCallPositionReady();
                }, 1.f, false);
        }
    }

}

void APlayerPawn::TriggerSurrender()
{
    TriggerSurrender_Server(HasAuthority());
}

void APlayerPawn::TriggerSurrender_Server_Implementation(bool bServer)
{
    XRGamePlayMode = (XRGamePlayMode == nullptr) ? Cast<AXRGamePlayMode>(UGameplayStatics::GetGameMode(this)) : XRGamePlayMode;
    if (XRGamePlayMode)
    {
        XRGamePlayMode->TriggerSurrender(bServer);
    }
}

void APlayerPawn::ServerGameModeCallPositionReady_Implementation()
{
    if (HasAuthority())
    {
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("Multi Test GameModeCallPositionReady")));
        }

        XRGamePlayMode = (XRGamePlayMode == nullptr) ? Cast<AXRGamePlayMode>(UGameplayStatics::GetGameMode(this)) : XRGamePlayMode;
        if (XRGamePlayMode)
        {
            XRGamePlayMode->PlayerPositionSetReady();
        }

        if (PlayerController)
        {
            SetUIOffenceDefence_Multi(PlayerController->controllerObjectType);
        }
    }
}

void APlayerPawn::SetUIOffenceDefence_Multi_Implementation(EObjectType objectType)
{
    SetUIOffenceDefence(objectType);
}
