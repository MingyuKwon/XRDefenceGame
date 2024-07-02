// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Deffence/MyXR_CharacterDeffenceBuff.h"

AMyXR_CharacterDeffenceBuff::AMyXR_CharacterDeffenceBuff()
{
    RingMeshComponent1 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RingMeshComponent1"));
    RingMeshComponent1->SetupAttachment(GetMesh());
    RingMeshComponent1->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    RingMeshComponent2 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RingMeshComponent2"));
    RingMeshComponent2->SetupAttachment(GetMesh());
    RingMeshComponent2->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    RingMeshComponent3 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RingMeshComponent3"));
    RingMeshComponent3->SetupAttachment(GetMesh());
    RingMeshComponent3->SetCollisionEnabled(ECollisionEnabled::NoCollision);

}

void AMyXR_CharacterDeffenceBuff::BeginPlay()
{
    Super::BeginPlay();
}

void AMyXR_CharacterDeffenceBuff::BindDissolveCallBack()
{
    InterpFunction.BindDynamic(this, &AMyXR_CharacterDeffenceBuff::DissolveCallBack);
}

void AMyXR_CharacterDeffenceBuff::DissolveCallBack(float percent)
{
    if (RingMeshComponent1->GetStaticMesh() != nullptr) RingMeshComponent1->SetScalarParameterValueOnMaterials("Dissolve", percent);
    if (RingMeshComponent2->GetStaticMesh() != nullptr) RingMeshComponent2->SetScalarParameterValueOnMaterials("Dissolve", percent);
    if (RingMeshComponent3->GetStaticMesh() != nullptr) RingMeshComponent3->SetScalarParameterValueOnMaterials("Dissolve", percent);

    Super::DissolveCallBack(1-percent);

}

