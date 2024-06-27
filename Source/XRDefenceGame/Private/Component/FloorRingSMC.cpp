// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/FloorRingSMC.h"
#include "Character/XR_Character.h"
#include "Materials/MaterialInstanceDynamic.h"

UFloorRingSMC::UFloorRingSMC()
{
	PrimaryComponentTick.bCanEverTick = true;

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

void UFloorRingSMC::BeginPlay()
{
	Super::BeginPlay();

	XRCharacter = Cast<AXR_Character>(GetOwner());

	if ( GetMaterial(0)) 
	{
		DynamicMaterialInstance = CreateAndSetMaterialInstanceDynamic(0);
	}
}

void UFloorRingSMC::SetMaterialScalarParameterValue(FName ParameterName, float ParameterValue)
{
	if (DynamicMaterialInstance)
	{
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
	GetWorld()->LineTraceSingleByChannel(FloortraceResult, ActorLocation, TraceEndLocation, beneathTraceChannel);

	FVector WillSpawnPosition = FloortraceResult.ImpactPoint - FVector(0.f, 0.f, 0.2f);
	
	FHitResult PallettetraceResult;
	GetWorld()->LineTraceSingleByChannel(PallettetraceResult, ActorLocation, TraceEndLocation, ECC_Pallette);


	bBeneathBoard = FloortraceResult.bBlockingHit && !PallettetraceResult.bBlockingHit;

	if (bBeneathBoard)
	{
		SetVisibility(true);
		SetWorldLocation(WillSpawnPosition);
	}
	else
	{
		SetVisibility(false);
		SetWorldLocation(ActorLocation);
	}
}