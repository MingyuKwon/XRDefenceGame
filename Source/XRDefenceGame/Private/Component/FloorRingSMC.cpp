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

	XRCharacter = Cast<AXR_Character>(GetOwner());
}

void UFloorRingSMC::SetMaterialScalarParameterValue(FName ParameterName, float ParameterValue)
{
	if (DynamicMaterialInstance)
	{
		UE_LOG(LogTemp, Display, TEXT("%f"), ParameterValue);
		DynamicMaterialInstance->SetScalarParameterValue(ParameterName, ParameterValue);
	}
}

void UFloorRingSMC::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (XRCharacter == nullptr) return;

	FVector ActorLocation = XRCharacter->GetActorLocation();
	FVector TraceEndLocation = ActorLocation - FVector(0.f, 0.f, traceLength);
		
	FHitResult FloortraceResult;

	if (IHandInteractInterface::Execute_IsOnBoard(Cast<UObject>(XRCharacter)))
	{
		GetWorld()->LineTraceSingleByChannel(FloortraceResult, ActorLocation, TraceEndLocation, ECC_Board);
	}
	else
	{
		GetWorld()->LineTraceSingleByChannel(FloortraceResult, ActorLocation, TraceEndLocation, beneathTraceChannel);
	}

	FVector WillSpawnPosition = FloortraceResult.ImpactPoint - FVector(0.f, 0.f, 0.f);
	
	FHitResult PallettetraceResult;
	GetWorld()->LineTraceSingleByChannel(PallettetraceResult, ActorLocation, TraceEndLocation, ECC_Pallette);


	bBeneathBoard = FloortraceResult.bBlockingHit && !PallettetraceResult.bBlockingHit;


	if (bBeneathBoard)
	{
		SetVisibility(true);
		SetWorldLocation(WillSpawnPosition);

		IBuffableInterface* beneathBuffable = Cast<IBuffableInterface>(FloortraceResult.GetActor());
		if (beneathBuffable)
		{
			AXR_Character* BeforeBuffableCharacter = BuffableCharacter;
			BuffableCharacter = Cast<AXR_Character>(FloortraceResult.GetActor());

			if (BeforeBuffableCharacter != BuffableCharacter)
			{
				IBuffableInterface::Execute_BuffableEffectStart(Cast<UObject>(BuffableCharacter));
				if (BeforeBuffableCharacter)
				{
					IBuffableInterface::Execute_BuffableEffectEnd(Cast<UObject>(BeforeBuffableCharacter));
				}
			}
		}
		else
		{
			BuffableCharacter = nullptr;
		}

	}
	else
	{
		SetVisibility(false);
		SetWorldLocation(ActorLocation);

		if (BuffableCharacter)
		{
			IBuffableInterface::Execute_BuffableEffectEnd(Cast<UObject>(BuffableCharacter));
		}
		BuffableCharacter = nullptr;

	}
}
