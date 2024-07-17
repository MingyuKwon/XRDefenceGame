// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/GoldMine.h"

AGoldMine::AGoldMine()
{
	GoldPile = CreateDefaultSubobject<UStaticMeshComponent>(FName("GoldMesh"));
	GoldPile->SetupAttachment(GetMesh());
	GoldPile->SetCollisionEnabled(ECollisionEnabled::NoCollision);

}

void AGoldMine::OnBoardCalledFunction(bool isOnBoard, bool isSpawnedByHand)
{
	Super::OnBoardCalledFunction(isOnBoard, isSpawnedByHand);
}

void AGoldMine::DissolveCallBack(float percent)
{
	if (GoldPile->GetStaticMesh() != nullptr) {
		GoldPile->SetScalarParameterValueOnMaterials("Dissolve", percent);
		GoldPile->SetScalarParameterValueOnMaterials("Dark", 1 - percent);
	}

	Super::DissolveCallBack(percent);
}

void AGoldMine::DissolveCallBackReverse(float percent)
{
	Super::DissolveCallBackReverse(percent);
	DissolveCallBack(1 - percent);

}
