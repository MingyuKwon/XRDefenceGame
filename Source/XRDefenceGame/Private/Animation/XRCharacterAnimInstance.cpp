// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/XRCharacterAnimInstance.h"
#include "Character/XR_Character.h"

void UXRCharacterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	APawn* OwningPawn = TryGetPawnOwner();
	if (!OwningPawn) return;

	AXR_Character* OwningCharacter = Cast<AXR_Character>(OwningPawn);
	if (!OwningCharacter) return;

	moveSpeed = OwningCharacter->GetVelocity().Size();
}
