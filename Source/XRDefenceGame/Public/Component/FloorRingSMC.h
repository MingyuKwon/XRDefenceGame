// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/StaticMeshComponent.h"
#include "XRDefenceGame/XRDefenceGame.h"
#include "FloorRingSMC.generated.h"

/**
 * 
 */
UCLASS()
class XRDEFENCEGAME_API UFloorRingSMC : public UStaticMeshComponent
{
	GENERATED_BODY()
	
public:
	UFloorRingSMC();

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bBeneathBoard;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TEnumAsByte<ECollisionChannel> beneathTraceChannel;

	UFUNCTION(BlueprintCallable)
	void ChangeRingColorRotation(float Percent, float SpinSpeed);

protected:
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TObjectPtr<class AXR_Character> XRCharacter;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	double traceLength = 50.f;



private:
	class UMaterialInstanceDynamic* DynamicMaterialInstance;

	void SetMaterialScalarParameterValue(FName ParameterName, float ParameterValue);


public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


};
