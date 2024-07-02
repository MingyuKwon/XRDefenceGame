// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "BuffableInterface.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class UBuffableInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class XRDEFENCEGAME_API IBuffableInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void BuffableEffectStart();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void BuffableEffectEnd();


};
