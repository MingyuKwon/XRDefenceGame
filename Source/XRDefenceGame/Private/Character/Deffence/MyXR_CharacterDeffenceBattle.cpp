// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Deffence/MyXR_CharacterDeffenceBattle.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"


AMyXR_CharacterDeffenceBattle::AMyXR_CharacterDeffenceBattle()
{
    PrimaryActorTick.bCanEverTick = true;

    GunMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("GunMeshComponent"));
    GunMeshComponent->SetupAttachment(GetMesh());
    GunMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);


    EtcMeshComponent1 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("EtcMeshComponent1"));
    EtcMeshComponent1->SetupAttachment(GetMesh());
    EtcMeshComponent1->SetCollisionEnabled(ECollisionEnabled::NoCollision);


    EtcMeshComponent2 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("EtcMeshComponent2"));
    EtcMeshComponent2->SetupAttachment(GetMesh());
    EtcMeshComponent2->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    EtcMeshComponent3 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("EtcMeshComponent3"));
    EtcMeshComponent3->SetupAttachment(GetMesh());
    EtcMeshComponent3->SetCollisionEnabled(ECollisionEnabled::NoCollision);


}
