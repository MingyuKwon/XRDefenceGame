// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/FloorRingSMC.h"
#include "Character/XR_Character.h"
#include "XRDefenceGame/XRDefenceGame.h"
#include "Interface/HandInteractInterface.h"
#include "Interface/BuffableInterface.h"
#include "Materials/MaterialInstanceDynamic.h"

UFloorRingSMC::UFloorRingSMC()
{
	PrimaryComponentTick.bCanEverTick = true;
	SetCollisionEnabled(ECollisionEnabled::NoCollision);
	

}

void UFloorRingSMC::ChangeRingColorRotation(float Percent, float SpinSpeed)
{
	if (DynamicMaterialInstance)
	{
		Percent = 1 - Percent;
		SetMaterialScalarParameterValue("ColorPercent", Percent);

		FRotator NewRotation(0.0f, SpinSpeed, 0.0f);
		AddLocalRotation(NewRotation);
	}
}

void UFloorRingSMC::SetMaterialCall()
{
	if (GetMaterial(0))
	{
		DynamicMaterialInstance = CreateAndSetMaterialInstanceDynamic(0);
	}
}

void UFloorRingSMC::BeginPlay()
{
	Super::BeginPlay();

	SetVisibility(false);
	XRCharacter = Cast<AXR_Character>(GetOwner());
}

void UFloorRingSMC::SetMaterialScalarParameterValue(FName ParameterName, float ParameterValue)
{
	if (DynamicMaterialInstance)
	{
		DynamicMaterialInstance->SetScalarParameterValue(ParameterName, ParameterValue);
	}
}

void UFloorRingSMC::CheckTrashBeneath(bool bBeneath, FHitResult& FloortraceResult)
{
	if (bBeneath)
	{
		FVector WillSpawnPosition = FloortraceResult.ImpactPoint - FVector(0.f, 0.f, 0.f);
		SetWorldLocation(WillSpawnPosition);
	}
	else
	{
		SetWorldLocation(XRCharacter->GetActorLocation());
	}

	if (XRCharacter->GebPalletteBeamAvailable()) XRCharacter->SetTrashEffect(bBeneath, true);

	if (bBeneath == bBeneathTrash) return;
	bBeneathTrash = bBeneath;
	bBeneathTrash ? SetVisibility(true) : SetVisibility(false);

}

void UFloorRingSMC::CheckBeneath(bool bBeneath, FHitResult& FloortraceResult)
{
	bBeneath = bBeneath && !XRCharacter->GetbDisableInteractable();

	if (bBeneath)
	{
		FVector WillSpawnPosition = FloortraceResult.ImpactPoint - FVector(0.f, 0.f, 0.f);
		SetWorldLocation(WillSpawnPosition);
	}
	else
	{
		SetWorldLocation(XRCharacter->GetActorLocation());
	}

	

	if (bBeneath == bBeneathBoard) return;
	bBeneathBoard = bBeneath;
	bBeneathBoard && beneathTraceChannel != ECC_Buffable ? SetVisibility(true) : SetVisibility(false);
}

void UFloorRingSMC::CheckBuffable(bool bBuffable, FHitResult& FloortraceResult)
{
	AXR_Character* NewBuffableCharacter = Cast<AXR_Character>(FloortraceResult.GetActor());

	bool isMaxLevel = false;
	bool isHeal = ownerCharacterType == ECharacterType::ECT_DefenceH;
	bool isOnBoard = true;

	if (NewBuffableCharacter)
	{
		isMaxLevel = IBuffableInterface::Execute_GetTotalLevel(NewBuffableCharacter) >= 6;
		isOnBoard = IHandInteractInterface::Execute_IsOnBoard(NewBuffableCharacter);
	}


	if (bBuffable && isOnBoard && (!isMaxLevel || isHeal))
	{
		if (NewBuffableCharacter != BuffableCharacter)
		{
			if (BuffableCharacter)
			{
				IBuffableInterface::Execute_BuffableEffectEnd(Cast<UObject>(BuffableCharacter));
			}
			BuffableCharacter = NewBuffableCharacter;
			IBuffableInterface::Execute_BuffableEffectStart(Cast<UObject>(BuffableCharacter));
		}

	}
	else
	{
		if (BuffableCharacter)
		{
			IBuffableInterface::Execute_BuffableEffectEnd(Cast<UObject>(BuffableCharacter));
		}
		BuffableCharacter = nullptr;

		if (isMaxLevel && ownerCharacterType != ECharacterType::ECT_DefenceH)
		{
			bBeneathBoard = false;
		}

	}

}

void UFloorRingSMC::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bTickReject) return;
	if (XRCharacter == nullptr) return;

	FVector ActorLocation = XRCharacter->GetActorLocation();
	FVector TraceEndLocation = ActorLocation - FVector(0.f, 0.f, traceLength);
		
	FHitResult FloortraceResult;

	if (bCharacterOnBoard)
	{
		GetWorld()->LineTraceSingleByChannel(FloortraceResult, ActorLocation, TraceEndLocation, ECC_Board);
	}
	else
	{
		GetWorld()->LineTraceSingleByChannel(FloortraceResult, ActorLocation, TraceEndLocation, beneathTraceChannel);
	}

	FHitResult PallettetraceResult;
	GetWorld()->LineTraceSingleByChannel(PallettetraceResult, ActorLocation, TraceEndLocation, ECC_Pallette);

	CheckBeneath(FloortraceResult.bBlockingHit && !PallettetraceResult.bBlockingHit, FloortraceResult);
	CheckBuffable(Cast<IBuffableInterface>(FloortraceResult.GetActor()) != nullptr, FloortraceResult);

	if (bBeneathBoard) return;

	GetWorld()->LineTraceSingleByChannel(FloortraceResult, ActorLocation, TraceEndLocation, ECC_Trash);
	CheckTrashBeneath(FloortraceResult.bBlockingHit, FloortraceResult);

}
