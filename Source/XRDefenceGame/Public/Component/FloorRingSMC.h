// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/StaticMeshComponent.h"
#include "XRDefenceGame/XRDefenceGame.h"
#include "FloorRingSMC.generated.h"

class AXR_Character;
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

	UFUNCTION(BlueprintCallable)
	void SetMaterialCall();

	bool bTickReject = false;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	TObjectPtr<AXR_Character> XRCharacter;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	double traceLength = 50.f;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	TObjectPtr<AXR_Character> BuffableCharacter;


private:
	class UMaterialInstanceDynamic* DynamicMaterialInstance;

	void SetMaterialScalarParameterValue(FName ParameterName, float ParameterValue);

	void CheckBeneath(bool bBeneath, FHitResult& FloortraceResult);
	void CheckBuffable(bool bBuffable, FHitResult& FloortraceResult);


public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	inline TObjectPtr<AXR_Character> GetBuffableCharacter() { return BuffableCharacter; }

};
