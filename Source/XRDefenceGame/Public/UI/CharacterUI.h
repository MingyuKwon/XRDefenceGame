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

	UFUNCTION(NetMulticast, Reliable)
	void SetHealthPercentMulticast(float percent);


	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetDamageCount(int32 count);

	UFUNCTION(NetMulticast, Reliable)
	void SetDamageCountMulticast(int32 count);


	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetUtilCount(int32 count);

	UFUNCTION(NetMulticast, Reliable)
	void SetUtilCountMulticast(int32 count);


	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetDamgeUtilVisible(bool flag);

	UFUNCTION(NetMulticast, Reliable)
	void SetDamgeUtilVisibleMulticast(bool flag);

};
