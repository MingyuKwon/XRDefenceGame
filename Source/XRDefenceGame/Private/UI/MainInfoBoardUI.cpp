// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MainInfoBoardUI.h"
#include "Components/TextBlock.h"
#include "Components/CanvasPanel.h"
#include "Components/Image.h"

void UMainInfoBoardUI::SetConnectState(bool offence, bool defence)
{
    GameExceptEndPanel->SetVisibility(ESlateVisibility::Hidden);

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

void UMainInfoBoardUI::SetFinalResultPanel(float FirstNexusCount, float FirstNexusHealth, float FirstTimeLeft, float SecondNexusCount, float SecondNexusHealth, float SecondTimeLeft, 
    bool bServerFirstDefence, bool bServer, 
    bool bSurrenderTriggeredByServer, bool bSurrenderTriggeredByClient)
{
    if (bSurrenderTriggeredByServer || bSurrenderTriggeredByClient)
    {
        GameExceptEndPanel->SetVisibility(ESlateVisibility::Visible);

        if (bSurrenderTriggeredByServer)
        {
            if (bServer)
            {
                GameEnd_WinnerText->SetText(FText::FromString(FString("Enemy Win...")));
                ExceptEndText->SetText(FText::FromString(FString("You Surrender")));
                ExceptEndPanel->SetColorAndOpacity(FLinearColor::Red);

            }
            else
            {
                GameEnd_WinnerText->SetText(FText::FromString(FString("You Win!")));
                ExceptEndText->SetText(FText::FromString(FString("Enemy Surrender")));
            }
        }
        else
        {
            if (!bServer)
            {
                GameEnd_WinnerText->SetText(FText::FromString(FString("Enemy Win...")));
                ExceptEndText->SetText(FText::FromString(FString("You Surrender")));
                ExceptEndPanel->SetColorAndOpacity(FLinearColor::Red);

            }
            else
            {
                GameEnd_WinnerText->SetText(FText::FromString(FString("You Win!")));
                ExceptEndText->SetText(FText::FromString(FString("Enemy Surrender")));
            }
        }

        return;
    }

    if (bServer)
    {
        if (bServerFirstDefence)
        {
            GameEnd_FirstPlayerText->SetText(FText::FromString(FString("YOU")));
            GameEnd_SecondPlayerText->SetText(FText::FromString(FString("ENEMY")));
        }
        else
        {
            GameEnd_SecondPlayerText->SetText(FText::FromString(FString("YOU")));
            GameEnd_FirstPlayerText->SetText(FText::FromString(FString("ENEMY")));
        }

    }
    else
    {
        if (!bServerFirstDefence)
        {
            GameEnd_FirstPlayerText->SetText(FText::FromString(FString("YOU")));
            GameEnd_SecondPlayerText->SetText(FText::FromString(FString("ENEMY")));
        }
        else
        {
            GameEnd_SecondPlayerText->SetText(FText::FromString(FString("YOU")));
            GameEnd_FirstPlayerText->SetText(FText::FromString(FString("ENEMY")));
        }
    }


    if (FirstNexusCount == -1)
    {
        FirtsPlayerNexus_AmountText->SetText(FText::FromString(FString(" ")));
        FirtsPlayerNexus_HealthText->SetText(FText::FromString(FString(" ")));
        FirtsPlayer_TimeLeftText->SetText(FText::FromString(FString(" ")));

        SecondPlayerNexus_AmountText->SetText(FText::FromString(FString(" ")));
        SecondPlayerNexus_HealthText->SetText(FText::FromString(FString(" ")));
        SecondPlayer_TimeLeftText->SetText(FText::FromString(FString(" ")));

        GameEnd_WinnerText->SetText(FText::FromString(FString(" ")));
        return;
    }

    int32 Minutes = FMath::FloorToInt(FirstTimeLeft / 60.0f);
    int32 Seconds = FMath::FloorToInt(FMath::Fmod(FirstTimeLeft, 60.0f));
    FString TimeString = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);

    FText NexusCountTextValue = FText::AsNumber(FirstNexusCount);
    FText NexusHealthTextValue = FText::AsNumber(FirstNexusHealth);

    FirtsPlayerNexus_AmountText->SetText(NexusCountTextValue);
    FirtsPlayerNexus_HealthText->SetText(NexusHealthTextValue);
    FirtsPlayer_TimeLeftText->SetText(FText::FromString(TimeString));


    if (SecondNexusCount == -1)
    {
        SecondPlayerNexus_AmountText->SetText(FText::FromString(FString(" ")));
        SecondPlayerNexus_HealthText->SetText(FText::FromString(FString(" ")));
        SecondPlayer_TimeLeftText->SetText(FText::FromString(FString(" ")));
        GameEnd_WinnerText->SetText(FText::FromString(FString("Next Game will start in 10 Seconds")));

        return;
    }

    Minutes = FMath::FloorToInt(SecondTimeLeft / 60.0f);
    Seconds = FMath::FloorToInt(FMath::Fmod(SecondTimeLeft, 60.0f));
    TimeString = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);

    NexusCountTextValue = FText::AsNumber(SecondNexusCount);
    NexusHealthTextValue = FText::AsNumber(SecondNexusHealth);

    SecondPlayerNexus_AmountText->SetText(NexusCountTextValue);
    SecondPlayerNexus_HealthText->SetText(NexusHealthTextValue);
    SecondPlayer_TimeLeftText->SetText(FText::FromString(TimeString));


    CheckWhoIsWinner(FirstNexusCount, FirstNexusHealth, FirstTimeLeft, SecondNexusCount, SecondNexusHealth, SecondTimeLeft, bServerFirstDefence, bServer);

}

void UMainInfoBoardUI::CheckWhoIsWinner(int32 FirstNexusCount, int32 FirstNexusHealth, int32 FirstTimeLeft, int32 SecondNexusCount, int32 SecondNexusHealth, int32 SecondTimeLeft, bool bServerFirstDefence, bool bServer)
{
    FString FirstPlayer = FString();
    FString SecondPlayer = FString();

    if (bServer)
    {
        if (bServerFirstDefence)
        {
            FirstPlayer = FString("You Win!");
            SecondPlayer = FString("Enemy Win...");
        }
        else
        {
            SecondPlayer = FString("You Win!");
            FirstPlayer = FString("Enemy Win...");

        }

    }
    else
    {
        if (!bServerFirstDefence)
        {
            FirstPlayer = FString("You Win!");
            SecondPlayer = FString("Enemy Win...");
        }
        else
        {
            SecondPlayer = FString("You Win!");
            FirstPlayer = FString("Enemy Win...");
        }
    }

    if (FirstNexusCount != SecondNexusCount)
    {
        GameEnd_NexusAmount_BackGround->SetColorAndOpacity(FLinearColor::Red);

        if (FirstNexusCount < SecondNexusCount)
        {
            GameEnd_WinnerText->SetText(FText::FromString(SecondPlayer));
        }
        else
        {
            GameEnd_WinnerText->SetText(FText::FromString(FirstPlayer));
        }

        return;
    }

    GameEnd_NexusAmount_BackGround->SetColorAndOpacity(FLinearColor::Green);


    if (FirstNexusHealth != SecondNexusHealth)
    {
        GameEnd_NexusHealth_BackGround->SetColorAndOpacity(FLinearColor::Red);

        if (FirstNexusHealth < SecondNexusHealth)
        {
            GameEnd_WinnerText->SetText(FText::FromString(SecondPlayer));
        }
        else
        {
            GameEnd_WinnerText->SetText(FText::FromString(FirstPlayer));
        }

        return;
    }

    GameEnd_NexusHealth_BackGround->SetColorAndOpacity(FLinearColor::Green);


    if (FirstTimeLeft != SecondTimeLeft)
    {
        GameEnd_TimeLeft_BackGround->SetColorAndOpacity(FLinearColor::Red);

        if (FirstTimeLeft < SecondTimeLeft)
        {
            GameEnd_WinnerText->SetText(FText::FromString(SecondPlayer));
        }
        else
        {
            GameEnd_WinnerText->SetText(FText::FromString(FirstPlayer));
        }

        return;
    }

    GameEnd_TimeLeft_BackGround->SetColorAndOpacity(FLinearColor::Green);
    GameEnd_WinnerText->SetText(FText::FromString(FString("Draw")));

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
