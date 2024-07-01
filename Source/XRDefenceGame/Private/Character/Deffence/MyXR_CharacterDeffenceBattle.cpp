// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Deffence/MyXR_CharacterDeffenceBattle.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "NiagaraComponent.h"
#include "Component/FloorRingSMC.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialInstance.h"
#include "XRDefenceGame/XRDefenceGame.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "Mode/XRGamePlayMode.h"


AMyXR_CharacterDeffenceBattle::AMyXR_CharacterDeffenceBattle()
{
    PrimaryActorTick.bCanEverTick = true;

    GunMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("GunMeshComponent"));
    GunMeshComponent->SetupAttachment(GetMesh());
    GunMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);


    EtcMeshComponent1 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("EtcMeshComponent1"));
    EtcMeshComponent1->SetupAttachment(GunMeshComponent);
    EtcMeshComponent1->SetCollisionEnabled(ECollisionEnabled::NoCollision);


    EtcMeshComponent2 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("EtcMeshComponent2"));
    EtcMeshComponent2->SetupAttachment(GunMeshComponent);
    EtcMeshComponent2->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    EtcMeshComponent3 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("EtcMeshComponent3"));
    EtcMeshComponent3->SetupAttachment(GunMeshComponent);
    EtcMeshComponent3->SetCollisionEnabled(ECollisionEnabled::NoCollision);


}

void AMyXR_CharacterDeffenceBattle::InitializeCharacter()
{
    if (GunMeshComponent == nullptr) return;
    if (EtcMeshComponent1 == nullptr) return;
    if (EtcMeshComponent2 == nullptr) return;
    if (EtcMeshComponent3 == nullptr) return;

    DefaultGunMaterial = Cast<UMaterialInstance>(GunMeshComponent->GetMaterial(0));
    DefaultEtcMaterialFirst = Cast<UMaterialInstance>(EtcMeshComponent1->GetMaterial(0));
    DefaultEtcMaterialSecond = Cast<UMaterialInstance>(EtcMeshComponent2->GetMaterial(0));
    DefaultEtcMaterialThird = Cast<UMaterialInstance>(EtcMeshComponent3->GetMaterial(0));

    Super::InitializeCharacter();
}

void AMyXR_CharacterDeffenceBattle::BindDissolveCallBack()
{
    InterpFunction.BindDynamic(this, &AMyXR_CharacterDeffenceBattle::DissolveCallBack);
}

void AMyXR_CharacterDeffenceBattle::DissolveCallBack(float percent)
{
    GunMeshComponent->SetScalarParameterValueOnMaterials("Dissolve", percent);

    if (EtcMeshComponent1->GetStaticMesh() != nullptr) EtcMeshComponent1->SetScalarParameterValueOnMaterials("Dissolve", percent);
    if (EtcMeshComponent2->GetStaticMesh() != nullptr) EtcMeshComponent2->SetScalarParameterValueOnMaterials("Dissolve", percent);
    if (EtcMeshComponent3->GetStaticMesh() != nullptr) EtcMeshComponent3->SetScalarParameterValueOnMaterials("Dissolve", percent);

    Super::DissolveCallBack(percent);
}


void AMyXR_CharacterDeffenceBattle::InteractableEffectStart_Implementation()
{
    Super::InteractableEffectStart_Implementation();


    if (HighlightMaterial)
    {
        GunMeshComponent->SetMaterial(0, HighlightMaterial);
        EtcMeshComponent1->SetMaterial(0, HighlightMaterial);
        EtcMeshComponent2->SetMaterial(0, HighlightMaterial);
        EtcMeshComponent3->SetMaterial(0, HighlightMaterial);
    }


    FVector NewScale = GunMeshComponent->GetRelativeScale3D() * rescaleAmount;
    GunMeshComponent->SetRelativeScale3D(NewScale);

    NewScale = EtcMeshComponent1->GetRelativeScale3D() * rescaleAmount;
    EtcMeshComponent1->SetRelativeScale3D(NewScale);

    NewScale = EtcMeshComponent2->GetRelativeScale3D() * rescaleAmount;
    EtcMeshComponent2->SetRelativeScale3D(NewScale);

    NewScale = EtcMeshComponent3->GetRelativeScale3D() * rescaleAmount;
    EtcMeshComponent3->SetRelativeScale3D(NewScale);

}

void AMyXR_CharacterDeffenceBattle::InteractableEffectEnd_Implementation()
{
    Super::InteractableEffectEnd_Implementation();

    if (DefaultGunMaterial) GunMeshComponent->SetMaterial(0, DefaultGunMaterial);
    if (DefaultEtcMaterialFirst) EtcMeshComponent1->SetMaterial(0, DefaultEtcMaterialFirst);
    if (DefaultEtcMaterialSecond) EtcMeshComponent2->SetMaterial(0, DefaultEtcMaterialSecond);
    if (DefaultEtcMaterialThird) EtcMeshComponent3->SetMaterial(0, DefaultEtcMaterialThird);


    FVector NewScale = GunMeshComponent->GetRelativeScale3D() / rescaleAmount;
    GunMeshComponent->SetRelativeScale3D(NewScale);

    NewScale = EtcMeshComponent1->GetRelativeScale3D() / rescaleAmount;
    EtcMeshComponent1->SetRelativeScale3D(NewScale);

    NewScale = EtcMeshComponent2->GetRelativeScale3D() / rescaleAmount;
    EtcMeshComponent2->SetRelativeScale3D(NewScale);

    NewScale = EtcMeshComponent3->GetRelativeScale3D() / rescaleAmount;
    EtcMeshComponent3->SetRelativeScale3D(NewScale);

}


