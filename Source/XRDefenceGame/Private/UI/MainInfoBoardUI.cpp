// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MainInfoBoardUI.h"
#include "Components/TextBlock.h"
#include "Components/CanvasPanel.h"


void UMainInfoBoardUI::SetTimeText(float LeftSecond)
{
    int32 Minutes = FMath::FloorToInt(LeftSecond / 60.0f);
    int32 Seconds = FMath::FloorToInt(FMath::Fmod(LeftSecond, 60.0f));
    FString TimeString = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
    FText TimeTextValue = FText::FromString(TimeString);
    if (TimeText)
    {
        TimeText->SetText(TimeTextValue);
    }
}

void UMainInfoBoardUI::SetBlueHeartText(float HealthAmount)
{
    FText BlueHeartTextValue = FText::AsNumber(HealthAmount);
    if (BlueHeartText)
    {
        BlueHeartText->SetText(BlueHeartTextValue);
    }
}

void UMainInfoBoardUI::SetPurpleHeartText(float HealthAmount)
{
    FText PurpleHeartTextValue = FText::AsNumber(HealthAmount);
    if (PurpleHeartText)
    {
        PurpleHeartText->SetText(PurpleHeartTextValue);
    }
}

void UMainInfoBoardUI::SetOrangeHeartText(float HealthAmount)
{
    FText OrangeHeartTextValue = FText::AsNumber(HealthAmount);
    if (OrangeHeartText)
    {
        OrangeHeartText->SetText(OrangeHeartTextValue);
    }
}

void UMainInfoBoardUI::SetHealthText(float HealthAmount)
{
    FText HealthTextValue = FText::AsNumber(HealthAmount);
    if (HealthText)
    {
        HealthText->SetText(HealthTextValue);
    }
}

void UMainInfoBoardUI::WhichPanelToShow(EGameMatchState matchState)
{
    switch (matchState)
    {
    case EGameMatchState::EGMS_FIrstGameWait :
    case EGameMatchState::EGMS_FIrstGameStart:
    case EGameMatchState::EGMS_SecondGameWait:
    case EGameMatchState::EGMS_SecondGameStart:
        GameStartPanel->SetVisibility(ESlateVisibility::Visible);
            break;

    case EGameMatchState::EGMS_FIrstGameEnd:
    case EGameMatchState::EGMS_SecondGameEnd:
        GameStartPanel->SetVisibility(ESlateVisibility::Hidden);
        break;

    case EGameMatchState::EGMS_FIrstGamePlaying:
    case EGameMatchState::EGMS_SecondGamePlaying:
        GameStartPanel->SetVisibility(ESlateVisibility::Hidden);
        break;

    }


}

void UMainInfoBoardUI::SetGameStart_GameStateText(FString text)
{
    if (GameStart_GameStateText)
    {
        GameStart_GameStateText->SetText(FText::FromString(text));
    }
}
