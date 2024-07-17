// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/GoldMine.h"

AGoldMine::AGoldMine()
{
	GoldPile = CreateDefaultSubobject<UStaticMeshComponent>(FName("GoldMesh"));
	GoldPile->SetupAttachment(GetMesh());
	GoldPile->SetCollisionEnabled(ECollisionEnabled::NoCollision);

}
