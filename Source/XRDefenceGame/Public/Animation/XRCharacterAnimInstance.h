// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "XRCharacterAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class XRDEFENCEGAME_API UXRCharacterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadWrite)
	float moveSpeed = 0.f;

protected:
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

};
