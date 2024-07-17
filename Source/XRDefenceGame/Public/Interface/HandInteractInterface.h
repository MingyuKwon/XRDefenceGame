#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "HandInteractInterface.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class UHandInteractInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 *
 */
class XRDEFENCEGAME_API IHandInteractInterface
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void InteractableEffectStart();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void InteractableEffectEnd();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void InteractStart();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void InteractEnd();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void SetInteractPosition(FVector GrabPosition);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void GrabStart();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void GrabEnd();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	bool IsOnBoard();


	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	float GetCost();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void SetDisableHighLight(bool bDisable);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	bool GetDisableHighLight();

};