#include "UI/GamePlayHandUI.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"

void UGamePlayHandUI::SetGoldText(float GoldAmount, float MaxGoldAmount)
{
    int32 GoldInt = GoldAmount;
    int32 MaxGoldInt = MaxGoldAmount;

    FString GoldString = FString::Printf(TEXT("%d / %d"), GoldInt, MaxGoldInt);
    FText GoldTextValue = FText::FromString(GoldString);

    if (GoldText)
    {
        GoldText->SetText(GoldTextValue);
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
        FString GestureCountDown = FString::Printf(TEXT("%d"), (int32)((1- precent) * 5));
        FText GestureCountDownText = FText::FromString(GestureCountDown);

        CoolTimeText->SetText(GestureCountDownText);
    }


}
