// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CharacterUI.generated.h"

UCLASS()
class XRDEFENCEGAME_API ACharacterUI : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACharacterUI();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetHealthPercent(float percent);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetDamageCount(int32 count);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetUtilCount(int32 count);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetDamgeUtilVisible(bool flag);

};
