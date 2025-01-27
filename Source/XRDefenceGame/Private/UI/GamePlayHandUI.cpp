#include "UI/GamePlayHandUI.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"

void UGamePlayHandUI::SetGoldText(float GoldAmount, float MaxGoldAmount, float GoldMine)
{
    int32 GoldInt = GoldAmount;
    int32 MaxGoldInt = MaxGoldAmount;

    FString GoldString = FString::Printf(TEXT("%d / %d"), GoldInt, MaxGoldInt);
    FText GoldTextValue = FText::FromString(GoldString);

    if (GoldText)
    {
        GoldText->SetText(GoldTextValue);
    }

    if (currentGoldMineText)
    {
        FString GoldMineString = FString::Printf(TEXT("%d"), (int32)GoldMine);
        FText GoldMineText = FText::FromString(GoldMineString);
        currentGoldMineText->SetText(GoldMineText);
    }
}

void UGamePlayHandUI::SetCoolTimeProgressBar(float precent)
{
    if (CoolTimeProgressBar)
    {
        CoolTimeProgressBar->SetPercent(precent);
    }

    if (CoolTimeText)
    {
        FString GestureCountDown = FString::Printf(TEXT("%d"), 5 - (int32)(precent * 5));
        FText GestureCountDownText = FText::FromString(GestureCountDown);

        CoolTimeText->SetText(GestureCountDownText);
    }


}

void UGamePlayHandUI::SetOffenceDefenceText(EObjectType objectType)
{
    if (objectType == EObjectType::EOT_Offence)
    {
        OffenceDefenceText->SetText(FText::FromString(FString("Offence")));
    }
    else if (objectType == EObjectType::EOT_Deffence)
    {
        OffenceDefenceText->SetText(FText::FromString(FString("Defence")));
    }

    
}
