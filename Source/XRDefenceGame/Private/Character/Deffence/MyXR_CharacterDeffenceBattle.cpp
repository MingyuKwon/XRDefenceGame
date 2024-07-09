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
#include "UI/CharacterUI.h"
#include "Mode/XRGamePlayMode.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Battle/Projectile.h"
#include "NiagaraFunctionLibrary.h"


void AMyXR_CharacterDeffenceBattle::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (bOnBoard)
    {
        if (TargetCharacter)
        {
            FVector StartLocation = GunMeshComponent->GetComponentLocation();
            FVector TargetLocation = TargetCharacter->GetActorLocation();
            FVector Direction = TargetLocation - StartLocation;
            Direction.Z = 0;
            Direction.Normalize();

            FRotator TargetRot = Direction.Rotation();
            TargetRotation = TargetRot;

        }
        else
        {
            TargetRotation = DefaultTargetRotation;
        }

        FRotator InterpRotation = FMath::RInterpTo(GetActorRotation(), TargetRotation, DeltaTime, 5.f);
        SetActorRotation(InterpRotation);
    }
}

AMyXR_CharacterDeffenceBattle::AMyXR_CharacterDeffenceBattle()
{
    PrimaryActorTick.bCanEverTick = true;

    GunMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("GunMeshComponent"));
    GunMeshComponent->SetupAttachment(GetMesh());
    GunMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    GunMeshComponent2 = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("GunMeshComponent2"));
    GunMeshComponent2->SetupAttachment(GunMeshComponent);
    GunMeshComponent2->SetCollisionEnabled(ECollisionEnabled::NoCollision);


    EtcMeshComponent1 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("EtcMeshComponent1"));
    EtcMeshComponent1->SetupAttachment(GunMeshComponent);
    EtcMeshComponent1->SetCollisionEnabled(ECollisionEnabled::NoCollision);


    EtcMeshComponent2 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("EtcMeshComponent2"));
    EtcMeshComponent2->SetupAttachment(GunMeshComponent);
    EtcMeshComponent2->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    EtcMeshComponent3 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("EtcMeshComponent3"));
    EtcMeshComponent3->SetupAttachment(GunMeshComponent);
    EtcMeshComponent3->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    EtcMeshComponent4 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("EtcMeshComponent4"));
    EtcMeshComponent4->SetupAttachment(GunMeshComponent);
    EtcMeshComponent4->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    EtcMeshComponent5 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("EtcMeshComponent5"));
    EtcMeshComponent5->SetupAttachment(GunMeshComponent);
    EtcMeshComponent5->SetCollisionEnabled(ECollisionEnabled::NoCollision);


    GetMesh()->SetCollisionResponseToChannel(ECC_Buffable, ECollisionResponse::ECR_Block);

}

void AMyXR_CharacterDeffenceBattle::InitializeCharacter()
{
    if (GunMeshComponent == nullptr) return;
    if (GunMeshComponent2 == nullptr) return;
    if (EtcMeshComponent1 == nullptr) return;
    if (EtcMeshComponent2 == nullptr) return;
    if (EtcMeshComponent3 == nullptr) return;
    if (EtcMeshComponent4 == nullptr) return;
    if (EtcMeshComponent5 == nullptr) return;

    DefaultGunMaterial = Cast<UMaterialInstance>(GunMeshComponent->GetMaterial(0));
    DefaultGun2Material = Cast<UMaterialInstance>(GunMeshComponent2->GetMaterial(0));
    DefaultEtcMaterialFirst = Cast<UMaterialInstance>(EtcMeshComponent1->GetMaterial(0));
    DefaultEtcMaterialSecond = Cast<UMaterialInstance>(EtcMeshComponent2->GetMaterial(0));
    DefaultEtcMaterialThird = Cast<UMaterialInstance>(EtcMeshComponent3->GetMaterial(0));
    DefaultEtcMaterialForth = Cast<UMaterialInstance>(EtcMeshComponent4->GetMaterial(0));
    DefaultEtcMaterialFifth = Cast<UMaterialInstance>(EtcMeshComponent5->GetMaterial(0));

    DefaultTargetRotation = GetActorRotation();
    TargetRotation = DefaultTargetRotation;

    Super::InitializeCharacter();
}

void AMyXR_CharacterDeffenceBattle::BindDissolveCallBack()
{
    InterpFunction.BindDynamic(this, &AMyXR_CharacterDeffenceBattle::DissolveCallBack);
}

void AMyXR_CharacterDeffenceBattle::DissolveCallBack(float percent)
{
    GunMeshComponent->SetScalarParameterValueOnMaterials("Dissolve", percent);

    if (GunMeshComponent2->GetSkeletalMeshAsset() != nullptr) GunMeshComponent2->SetScalarParameterValueOnMaterials("Dissolve", percent);

    if (EtcMeshComponent1->GetStaticMesh() != nullptr) EtcMeshComponent1->SetScalarParameterValueOnMaterials("Dissolve", percent);
    if (EtcMeshComponent2->GetStaticMesh() != nullptr) EtcMeshComponent2->SetScalarParameterValueOnMaterials("Dissolve", percent);
    if (EtcMeshComponent3->GetStaticMesh() != nullptr) EtcMeshComponent3->SetScalarParameterValueOnMaterials("Dissolve", percent);
    if (EtcMeshComponent4->GetStaticMesh() != nullptr) EtcMeshComponent4->SetScalarParameterValueOnMaterials("Dissolve", percent);
    if (EtcMeshComponent5->GetStaticMesh() != nullptr) EtcMeshComponent5->SetScalarParameterValueOnMaterials("Dissolve", percent);

    Super::DissolveCallBack(percent);
}

void AMyXR_CharacterDeffenceBattle::DissolveCallBackReverse(float percent)
{
    Super::DissolveCallBackReverse(percent);
    DissolveCallBack(1-percent);
}

void AMyXR_CharacterDeffenceBattle::HighLightMesh(bool bHighlight)
{
    Super::HighLightMesh(bHighlight);


    if (bHighlight)
    {
        if (HighlightMaterial)
        {
            GunMeshComponent->SetMaterial(0, HighlightMaterial);
            GunMeshComponent2->SetMaterial(0, HighlightMaterial);

            EtcMeshComponent1->SetMaterial(0, HighlightMaterial);
            EtcMeshComponent2->SetMaterial(0, HighlightMaterial);
            EtcMeshComponent3->SetMaterial(0, HighlightMaterial);
            EtcMeshComponent4->SetMaterial(0, HighlightMaterial);
            EtcMeshComponent5->SetMaterial(0, HighlightMaterial);
        }

    }else
    {
        if (DefaultGunMaterial) GunMeshComponent->SetMaterial(0, DefaultGunMaterial);
        if (DefaultGun2Material) GunMeshComponent2->SetMaterial(0, DefaultGun2Material);

        if (DefaultEtcMaterialFirst) EtcMeshComponent1->SetMaterial(0, DefaultEtcMaterialFirst);
        if (DefaultEtcMaterialSecond) EtcMeshComponent2->SetMaterial(0, DefaultEtcMaterialSecond);
        if (DefaultEtcMaterialThird) EtcMeshComponent3->SetMaterial(0, DefaultEtcMaterialThird);
        if (DefaultEtcMaterialForth) EtcMeshComponent4->SetMaterial(0, DefaultEtcMaterialForth);
        if (DefaultEtcMaterialFifth) EtcMeshComponent5->SetMaterial(0, DefaultEtcMaterialFifth);
    }
}



void AMyXR_CharacterDeffenceBattle::InteractableEffectStart_Implementation()
{
    Super::InteractableEffectStart_Implementation();

    HighLightMesh(true);

    FVector NewScale = GunMeshComponent->GetRelativeScale3D() * rescaleAmount;
    GunMeshComponent->SetRelativeScale3D(NewScale);

    NewScale = GunMeshComponent2->GetRelativeScale3D() * rescaleAmount;
    GunMeshComponent2->SetRelativeScale3D(NewScale);


    NewScale = EtcMeshComponent1->GetRelativeScale3D() * rescaleAmount;
    EtcMeshComponent1->SetRelativeScale3D(NewScale);

    NewScale = EtcMeshComponent2->GetRelativeScale3D() * rescaleAmount;
    EtcMeshComponent2->SetRelativeScale3D(NewScale);

    NewScale = EtcMeshComponent3->GetRelativeScale3D() * rescaleAmount;
    EtcMeshComponent3->SetRelativeScale3D(NewScale);

    NewScale = EtcMeshComponent4->GetRelativeScale3D() * rescaleAmount;
    EtcMeshComponent4->SetRelativeScale3D(NewScale);

    NewScale = EtcMeshComponent5->GetRelativeScale3D() * rescaleAmount;
    EtcMeshComponent5->SetRelativeScale3D(NewScale);

}

void AMyXR_CharacterDeffenceBattle::InteractableEffectEnd_Implementation()
{
    Super::InteractableEffectEnd_Implementation();

    HighLightMesh(false);


    FVector NewScale = GunMeshComponent->GetRelativeScale3D() / rescaleAmount;
    GunMeshComponent->SetRelativeScale3D(NewScale);

    NewScale = GunMeshComponent2->GetRelativeScale3D() / rescaleAmount;
    GunMeshComponent2->SetRelativeScale3D(NewScale);


    NewScale = EtcMeshComponent1->GetRelativeScale3D() / rescaleAmount;
    EtcMeshComponent1->SetRelativeScale3D(NewScale);

    NewScale = EtcMeshComponent2->GetRelativeScale3D() / rescaleAmount;
    EtcMeshComponent2->SetRelativeScale3D(NewScale);

    NewScale = EtcMeshComponent3->GetRelativeScale3D() / rescaleAmount;
    EtcMeshComponent3->SetRelativeScale3D(NewScale);

    NewScale = EtcMeshComponent4->GetRelativeScale3D() / rescaleAmount;
    EtcMeshComponent4->SetRelativeScale3D(NewScale);

    NewScale = EtcMeshComponent5->GetRelativeScale3D() / rescaleAmount;
    EtcMeshComponent5->SetRelativeScale3D(NewScale);

}

void AMyXR_CharacterDeffenceBattle::BuffableEffectStart_Implementation()
{
    if (bHightLighting) return;

    bBufferHightLighting = true;

    HighLightMesh(true);
}

void AMyXR_CharacterDeffenceBattle::BuffableEffectEnd_Implementation()
{
    bBufferHightLighting = false;

    if (bHightLighting) return;

    HighLightMesh(false);

}

void AMyXR_CharacterDeffenceBattle::BuffApplied_Implementation(ECharacterType buffType)
{
    if (ECharacterType::ECT_DefenceH == buffType)
    {
        return;
    }
    if (GetTotalLevel_Implementation() == 6) return;


    switch (buffType)
    {
    case ECharacterType::ECT_DefenceF1 :
        DamageUpgradeCount++;
        break;

    case ECharacterType::ECT_DefenceF2:
        RangeUpgradeCount++;
        break;
    }

    ECharacterType* upgradeTurretTypePtr = TurretTypeMap.Find(GetUpgradeLevel_Implementation());
    if (upgradeTurretTypePtr)
    {
        UpgradeTurret(*upgradeTurretTypePtr);
    }

    UpdateCharacterPropertyUI();
    
}

void AMyXR_CharacterDeffenceBattle::UpgradeTurret(ECharacterType characterType)
{
    FCharacterValueTransmitForm outForm;
    PackCharacterValueTransmitForm(outForm);

    XRGamePlayMode->SpawnActorForUpgrade(characterType, GetActorLocation(), GetActorRotation(), outForm);
    DestroyMyself();
}

int32 AMyXR_CharacterDeffenceBattle::GetUpgradeLevel_Implementation()
{
    int32 returnValue = 0;
    returnValue += DamageUpgradeCount * 10;
    returnValue += RangeUpgradeCount;
    return returnValue;
}

int32 AMyXR_CharacterDeffenceBattle::GetTotalLevel_Implementation()
{
    return DamageUpgradeCount + RangeUpgradeCount;
}

void AMyXR_CharacterDeffenceBattle::NonPalletteSpawnInitalize(FCharacterValueTransmitForm inheritform)
{
    Super::NonPalletteSpawnInitalize(inheritform);

    DamageUpgradeCount = inheritform.DamageUpgradeCount;
    RangeUpgradeCount = inheritform.RangeUpgradeCount;

    UpdateCharacterPropertyUI();

}

void AMyXR_CharacterDeffenceBattle::CharacterActionImpact()
{
    Super::CharacterActionImpact();
    FireBullet();
}

void AMyXR_CharacterDeffenceBattle::CharacterActionImpact2()
{
    Super::CharacterActionImpact2();
    FireBullet(true);
}

void AMyXR_CharacterDeffenceBattle::FireBullet(bool isDouble)
{
    FName SockeName = isDouble ? FName("MuzzleSocket2") : FName("MuzzleSocket");

    const USkeletalMeshSocket* MuzzleSocket = GunMeshComponent->GetSocketByName(SockeName);
    if (MuzzleSocket && TargetCharacter)
    {
        FTransform MuzzleTransform = MuzzleSocket->GetSocketTransform(GunMeshComponent);
        FVector EndPosition = TargetCharacter->GetActorLocation();

        FHitResult BulletScan;
        GetWorld()->LineTraceSingleByChannel(BulletScan, MuzzleTransform.GetLocation(), EndPosition, ECC_Bullet);
        
        if (BulletScan.bBlockingHit)
        {
            EndPosition = BulletScan.ImpactPoint;
        }

        UGameplayStatics::ApplyDamage(TargetCharacter, CharacterProperty.Damage, GetController(), this, nullptr);

        if (trailBeam)
        {
            UNiagaraComponent* beamTrailNiagara = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), trailBeam, MuzzleTransform.GetLocation(), FRotator::ZeroRotator, FVector(1.0f), true);

            if (beamTrailNiagara)
            {
                beamTrailNiagara->SetVariableVec3(FName("TrailStart"), MuzzleTransform.GetLocation());
                beamTrailNiagara->SetVariableVec3(FName("TrailEnd"), EndPosition);
            }

            if (shootParticle)
            {
                UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), shootParticle, MuzzleTransform.GetLocation(), FRotator::ZeroRotator, FVector(1.0f), true);
            }
        }

 
    }
}

void AMyXR_CharacterDeffenceBattle::PackCharacterValueTransmitForm(FCharacterValueTransmitForm& outForm)
{
    Super::PackCharacterValueTransmitForm(outForm);
    outForm.DamageUpgradeCount = DamageUpgradeCount;
    outForm.RangeUpgradeCount = RangeUpgradeCount;
}

void AMyXR_CharacterDeffenceBattle::UpdateCharacterPropertyUI()
{
    Super::UpdateCharacterPropertyUI();

    if (CharacterPropertyUI)
    {
        CharacterPropertyUI->SetDamageCount(DamageUpgradeCount);
        CharacterPropertyUI->SetUtilCount(RangeUpgradeCount);
    }
}

void AMyXR_CharacterDeffenceBattle::CharacterActionStart()
{
    if (GunFireMontage && GunMeshComponent->GetAnimInstance())
    {
        SetAnimState(EAnimationState::EAS_Action);
        GunMeshComponent->GetAnimInstance()->Montage_Play(GunFireMontage);

        if (GunMeshComponent2->GetSkeletalMeshAsset() && GunMeshComponent2->GetAnimInstance())
        {
            GunMeshComponent2->GetAnimInstance()->Montage_Play(GunFireMontage);
        }
    }



}


