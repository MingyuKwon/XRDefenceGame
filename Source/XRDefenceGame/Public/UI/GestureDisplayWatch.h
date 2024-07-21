// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GestureDisplayWatch.generated.h"

class UProgressBar;
/**
 * 
 */
UCLASS()
class XRDEFENCEGAME_API UGestureDisplayWatch : public UUserWidget
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(meta = (BindWidget))
	UProgressBar* CoolTimeProgressBar;

public:
	UFUNCTION(BlueprintCallable)
	void SetCoolTimeProgressBar(float precent);

};
