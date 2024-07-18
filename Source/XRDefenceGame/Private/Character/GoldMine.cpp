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

void AGoldMine::InitializeCharacter()
{
    if (!GoldPile) return;

    DefaultGoldPile = Cast<UMaterialInstance>(GoldPile->GetMaterial(0));

    Super::InitializeCharacter();
}

void AGoldMine::ChangeMaterialEMS_Default()
{
    Super::ChangeMaterialEMS_Default();

    if (GoldPile) GoldPile->SetMaterial(0, DefaultGoldPile);

}

void AGoldMine::ChangeMaterialEMS_OnBoardHighLight()
{
    Super::ChangeMaterialEMS_OnBoardHighLight();

    if (HighlightMaterial)
    {
        GoldPile->SetMaterial(0, HighlightMaterial);
    }


}

void AGoldMine::ChangeMaterialEMS_Damage()
{
    Super::ChangeMaterialEMS_Damage();
    if (DamagedMaterial)
    {
        GoldPile->SetMaterial(0, DamagedMaterial);
    }
}

void AGoldMine::ChangeMaterialEMS_HandHighLight()
{
    Super::ChangeMaterialEMS_HandHighLight();

    if (bDisableInteractable)
    {
        if (DisableHighlightMaterial)
        {
            GoldPile->SetMaterial(0, DisableHighlightMaterial);
        }
    }
    else
    {
        if (HighlightMaterial)
        {
            GoldPile->SetMaterial(0, HighlightMaterial);
        }
    }
}

void AGoldMine::ChangeMaterialEMS_Death()
{
    Super::ChangeMaterialEMS_Death();

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
		EObjectType tempObjectType = ObjectType;
		if (ObjectType == EObjectType::EOT_OffenceGold) tempObjectType = EObjectType::EOT_Offence;
		if (ObjectType == EObjectType::EOT_DeffenceGold) tempObjectType = EObjectType::EOT_Deffence;

		XRGamePlayMode->OnGoldMineBroadCastEvent.Broadcast(tempObjectType, !flag, 0.f);
	}
}

void AGoldMine::BroadCastGoldMineTick()
{
	if (XRGamePlayMode)
	{
		EObjectType tempObjectType = ObjectType;
		if (ObjectType == EObjectType::EOT_OffenceGold) tempObjectType = EObjectType::EOT_Offence;
		if (ObjectType == EObjectType::EOT_DeffenceGold) tempObjectType = EObjectType::EOT_Deffence;

		XRGamePlayMode->OnGoldMineBroadCastEvent.Broadcast(tempObjectType, false, PerSecGold);
	}
}

