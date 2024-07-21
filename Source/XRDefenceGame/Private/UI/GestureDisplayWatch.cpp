// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/GestureDisplayWatch.h"
#include "Components/ProgressBar.h"

void UGestureDisplayWatch::SetCoolTimeProgressBar(float precent)
{
	CoolTimeProgressBar->SetPercent(precent);
}
