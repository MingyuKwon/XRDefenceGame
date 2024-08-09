// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MainInfoBoardUI.h"
#include "Components/TextBlock.h"
#include "Components/CanvasPanel.h"

void UMainInfoBoardUI::SetConnectState(bool offence, bool defence)
{
    if (offence)
    {
        GameStart_OffenceConnectText->SetText(FText::FromString(FString("O")));
    }

    if (defence)
    {
        GameStart_DefenceConnectText->SetText(FText::FromString(FString("O")));
    }

    if (offence && defence)
    {
        SetGameStateText(EGameMatchState::EGMS_FIrstGameStart, FString("Get Ready For Match"));
    }

}

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
        GameEndtPanel->SetVisibility(ESlateVisibility::Hidden);
            break;

    case EGameMatchState::EGMS_FIrstGameEnd:
    case EGameMatchState::EGMS_SecondGameEnd:
        GameStartPanel->SetVisibility(ESlateVisibility::Hidden);
        GameEndtPanel->SetVisibility(ESlateVisibility::Visible);

        break;

    case EGameMatchState::EGMS_FIrstGamePlaying:
    case EGameMatchState::EGMS_SecondGamePlaying:
        GameStartPanel->SetVisibility(ESlateVisibility::Hidden);
        GameEndtPanel->SetVisibility(ESlateVisibility::Hidden);

        break;

    }


}

void UMainInfoBoardUI::SetGameStateText(EGameMatchState matchState, FString text)
{
    switch (matchState)
    {
    case EGameMatchState::EGMS_FIrstGameWait:
    case EGameMatchState::EGMS_FIrstGameStart:
    case EGameMatchState::EGMS_SecondGameWait:
    case EGameMatchState::EGMS_SecondGameStart:
        if (GameStart_GameStateText)
        {
            GameStart_GameStateText->SetText(FText::FromString(text));
        }
        break;

    case EGameMatchState::EGMS_FIrstGameEnd:
    case EGameMatchState::EGMS_SecondGameEnd:
        GameStartPanel->SetVisibility(ESlateVisibility::Hidden);
        GameEndtPanel->SetVisibility(ESlateVisibility::Visible);

        break;

    case EGameMatchState::EGMS_FIrstGamePlaying:
    case EGameMatchState::EGMS_SecondGamePlaying:
        GameStartPanel->SetVisibility(ESlateVisibility::Hidden);
        GameEndtPanel->SetVisibility(ESlateVisibility::Hidden);

        break;

    }


}
