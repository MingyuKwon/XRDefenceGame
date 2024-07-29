// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/PlayerPawn.h"
#include "Kismet/GameplayStatics.h"
#include "Character/Offence/MyXR_CharacterOffenceBattle.h"
#include "Character/Deffence/MyXR_CharacterDeffenceBattle.h"
#include "Interface/HandInteractInterface.h"
#include "Managet/XRDefenceGameInstance.h"
#include "Mode/XRGamePlayMode.h"
#include "Kismet/GameplayStatics.h"

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

    FVector MapSpawnLocation = GI->PlayerGamePlayLocation;
    FRotator MapSpawnRotation = GI->PlayerGamePlayRotation;


	FVector ReverseLocation = FVector::ZeroVector - MapSpawnLocation;
	FVector ShouldMovetoLocation = GetActorLocation() + ReverseLocation;
	SetActorLocation(ShouldMovetoLocation);

    FRotator ReverseRotation = MapSpawnRotation * -1;
    FVector RotatedVector = ReverseRotation.RotateVector(GetActorLocation());
    SetActorLocation(RotatedVector);

    FRotator NewRotation = GetActorRotation() + ReverseRotation;
    SetActorRotation(NewRotation);
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

void APlayerPawn::UpdateUserLeftHandUI_Implementation(float GoldAmount, float MaxGoldAmount, float TimeSecond, float TotalHealthAmount, float OrangeHealthAmount, float BlueHealthAmount, float PurpleHealthAmount, float GesturePercent)
{
    SetUIGoldAmount(GoldAmount, MaxGoldAmount);

    SetUIPurpleHealth(PurpleHealthAmount);
    SetUIOrnageHealth(OrangeHealthAmount);
    SetUBlueHealth(BlueHealthAmount);
    SetUIHealth(TotalHealthAmount);

    SetUITime(TimeSecond);

    SetUIGestureCoolTime(GesturePercent);

}

void APlayerPawn::BeginPlay()
{
    Super::BeginPlay();

    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("Multi Test BeginPlay()")));
    }

    SetPawnTransformForGameStart();
    ServerGameModeCallPositionReady();
}

void APlayerPawn::ServerGameModeCallPositionReady_Implementation()
{
    if (HasAuthority())
    {
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("Multi Test GameModeCallPositionReady")));
        }

        AXRGamePlayMode* GameMode = Cast<AXRGamePlayMode>(UGameplayStatics::GetGameMode(this));
        if (GameMode)
        {
            GameMode->PlayerPositionSetReady();
        }


    }
}
