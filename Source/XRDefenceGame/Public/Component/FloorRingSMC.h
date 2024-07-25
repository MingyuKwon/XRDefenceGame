// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/StaticMeshComponent.h"
#include "XRDefenceGame/XRDefenceGame.h"
#include "XRDefenceEnums.h"
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
	bool bBeneathTrash = false;


	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TEnumAsByte<ECollisionChannel> beneathTraceChannel;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TEnumAsByte<ECharacterType> ownerCharacterType;


	UFUNCTION(BlueprintCallable)
	void ChangeRingColorRotation(float Percent, float SpinSpeed);

	UFUNCTION(BlueprintCallable)
	void SetMaterialCall();

	bool bTickReject = false;
	UFUNCTION(NetMulticast, Reliable)
	void SetbTickReject(bool flag);


	bool bCharacterOnBoard = false;

	UFUNCTION(NetMulticast, Reliable)
	void SetbCharacterOnBoard(bool flag);


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

	void CheckTrashBeneath(bool bBeneath, FHitResult& FloortraceResult);
	void CheckBeneath(bool bBeneath, FHitResult& FloortraceResult);
	void CheckBuffable(bool bBuffable, FHitResult& FloortraceResult);


public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	inline TObjectPtr<AXR_Character> GetBuffableCharacter() { return BuffableCharacter; }

};
