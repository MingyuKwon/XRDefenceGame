#include "UI/GamePlayHandUI.h"
#include "Components/TextBlock.h"

void UGamePlayHandUI::SetTimeText(float LeftSecond)
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

void UGamePlayHandUI::SetGoldText(float GoldAmount, float MaxGoldAmount)
{
    int32 GoldInt = GoldAmount;
    int32 MaxGoldInt = MaxGoldAmount;

    FString GoldString = FString::Printf(TEXT("%d \n/ %d"), GoldInt, MaxGoldInt);
    FText GoldTextValue = FText::FromString(GoldString);

    if (GoldText)
    {
        GoldText->SetText(GoldTextValue);
    }
}

void UGamePlayHandUI::SetBlueHeartText(float HealthAmount)
{
    FText BlueHeartTextValue = FText::AsNumber(HealthAmount);
    if (BlueHeartText)
    {
        BlueHeartText->SetText(BlueHeartTextValue);
    }
}

void UGamePlayHandUI::SetPurpleHeartText(float HealthAmount)
{
    FText PurpleHeartTextValue = FText::AsNumber(HealthAmount);
    if (PurpleHeartText)
    {
        PurpleHeartText->SetText(PurpleHeartTextValue);
    }
}

void UGamePlayHandUI::SetOrangeHeartText(float HealthAmount)
{
    FText OrangeHeartTextValue = FText::AsNumber(HealthAmount);
    if (OrangeHeartText)
    {
        OrangeHeartText->SetText(OrangeHeartTextValue);
    }
}

void UGamePlayHandUI::SetHealthText(float HealthAmount)
{
    FText HealthTextValue = FText::AsNumber(HealthAmount);
    if (HealthText)
    {
        HealthText->SetText(HealthTextValue);
    }
}
