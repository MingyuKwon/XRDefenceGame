// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/GoldMine.h"
#include "Mode/XRGamePlayMode.h"

AGoldMine::AGoldMine()
{
	GoldPile = CreateDefaultSubobject<UStaticMeshComponent>(FName("GoldMesh"));
	GoldPile->SetupAttachment(GetMesh());
	GoldPile->SetCollisionEnabled(ECollisionEnabled::NoCollision);

}

void AGoldMine::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (TimeCount < 0) return;

	TimeCount += DeltaTime;

	if (TimeCount >= LifTime)
	{
		Death(false);
		TimeCount = -1.f;
	}

}

void AGoldMine::OnBoardCalledFunction(bool isOnBoard, bool isSpawnedByHand)
{
	Super::OnBoardCalledFunction(isOnBoard, isSpawnedByHand);

	BroadCastGoldMineOnBoard(true);
	GetWorld()->GetTimerManager().SetTimer(GoldMineTimerHandler, this, &AGoldMine::BroadCastGoldMineTick, 1.0f, true);
	TimeCount = 0.f;
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

void AGoldMine::Death(bool bDieInTrash)
{
	Super::Death(bDieInTrash);

	if (!bDieInTrash)
	{
		BroadCastGoldMineOnBoard(false);
		GetWorld()->GetTimerManager().ClearTimer(GoldMineTimerHandler);
	}
	
}

void AGoldMine::BroadCastGoldMineOnBoard(bool flag)
{
	if (XRGamePlayMode)
	{
		XRGamePlayMode->OnGoldMineBroadCastEvent.Broadcast(ObjectType, flag, 0.f);
	}
}

void AGoldMine::BroadCastGoldMineTick()
{
	if (XRGamePlayMode)
	{
		XRGamePlayMode->OnGoldMineBroadCastEvent.Broadcast(ObjectType, false, PerSecGold);
	}
}

