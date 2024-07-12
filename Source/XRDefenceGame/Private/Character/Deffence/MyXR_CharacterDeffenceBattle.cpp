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
#include "Character/Offence/MyXR_CharacterOffenceBattle.h"
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
            FVector StartLocation = GetActorLocation();
            FVector TargetLocation = TargetCharacter->GetActorLocation();
            FVector Direction = TargetLocation - StartLocation;
            Direction.Z = 0;
            Direction.Normalize();

            FRotator TargetRot = Direction.Rotation();
            TargetRotation = TargetRot;

            if (TargetCharacter2 && bAttackBoth)
            {
                FVector TargetLocation2 = TargetCharacter2->GetActorLocation();
                FVector Direction2 = TargetLocation2 - StartLocation;
                Direction2.Z = 0;
                Direction2.Normalize();

                FVector AverageDirection = (Direction + Direction2).GetSafeNormal();
                FRotator AverageRot = AverageDirection.Rotation();

                TargetRotation = AverageRot;
            }

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
    GunMeshComponent->SetScalarParameterValueOnMaterials("Dark", 1 - percent);

    if (GunMeshComponent2->GetSkeletalMeshAsset() != nullptr) {
        GunMeshComponent2->SetScalarParameterValueOnMaterials("Dissolve", percent);
        GunMeshComponent2->SetScalarParameterValueOnMaterials("Dark", 1 - percent);
    }        

    if (EtcMeshComponent1->GetStaticMesh() != nullptr) {
        EtcMeshComponent1->SetScalarParameterValueOnMaterials("Dissolve", percent);
        EtcMeshComponent1->SetScalarParameterValueOnMaterials("Dark", 1 - percent);

    }
        
    if (EtcMeshComponent2->GetStaticMesh() != nullptr) {
        EtcMeshComponent2->SetScalarParameterValueOnMaterials("Dissolve", percent);
        EtcMeshComponent2->SetScalarParameterValueOnMaterials("Dark", 1 - percent);

    }
        
    if (EtcMeshComponent3->GetStaticMesh() != nullptr) {
        EtcMeshComponent3->SetScalarParameterValueOnMaterials("Dissolve", percent);
        EtcMeshComponent3->SetScalarParameterValueOnMaterials("Dark", 1 - percent);

    }
        
    if (EtcMeshComponent4->GetStaticMesh() != nullptr) {
        EtcMeshComponent4->SetScalarParameterValueOnMaterials("Dissolve", percent);
        EtcMeshComponent4->SetScalarParameterValueOnMaterials("Dark", 1 - percent);

    }
        
    if (EtcMeshComponent5->GetStaticMesh() != nullptr)
    {
        EtcMeshComponent5->SetScalarParameterValueOnMaterials("Dissolve", percent);
        EtcMeshComponent5->SetScalarParameterValueOnMaterials("Dark", 1 - percent);

    }

    Super::DissolveCallBack(percent);
}

void AMyXR_CharacterDeffenceBattle::DissolveCallBackReverse(float percent)
{
    Super::DissolveCallBackReverse(percent);
    DissolveCallBack(1-percent);
}

void AMyXR_CharacterDeffenceBattle::ChangeMaterialState(EMaterialState materialState, bool bLock)
{
    Super::ChangeMaterialState(materialState, bLock);

    EMaterialState HightestState = EMaterialState::EMS_Default;
    if (bLockDeath)
    {
        HightestState = EMaterialState::EMS_Death;
    }
    else if (bLockHandHighLight)
    {
        HightestState = EMaterialState::EMS_HandHighLight;
    }
    else if (bLockDamage)
    {
        HightestState = EMaterialState::EMS_Damage;
    }
    else if (bLockOnBoardHighLight)
    {
        HightestState = EMaterialState::EMS_OnBoardHighLight;
    }

    switch (HightestState)
    {
    case EMaterialState::EMS_Default:
        if (DefaultGunMaterial) GunMeshComponent->SetMaterial(0, DefaultGunMaterial);
        if (DefaultGun2Material) GunMeshComponent2->SetMaterial(0, DefaultGun2Material);

        if (DefaultEtcMaterialFirst) EtcMeshComponent1->SetMaterial(0, DefaultEtcMaterialFirst);
        if (DefaultEtcMaterialSecond) EtcMeshComponent2->SetMaterial(0, DefaultEtcMaterialSecond);
        if (DefaultEtcMaterialThird) EtcMeshComponent3->SetMaterial(0, DefaultEtcMaterialThird);
        if (DefaultEtcMaterialForth) EtcMeshComponent4->SetMaterial(0, DefaultEtcMaterialForth);
        if (DefaultEtcMaterialFifth) EtcMeshComponent5->SetMaterial(0, DefaultEtcMaterialFifth);

        break;

    case EMaterialState::EMS_OnBoardHighLight:

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

        break;

    case EMaterialState::EMS_Damage:
        if (DamagedMaterial)
        {
            GunMeshComponent->SetMaterial(0, DamagedMaterial);
            GunMeshComponent2->SetMaterial(0, DamagedMaterial);

            EtcMeshComponent1->SetMaterial(0, DamagedMaterial);
            EtcMeshComponent2->SetMaterial(0, DamagedMaterial);
            EtcMeshComponent3->SetMaterial(0, DamagedMaterial);
            EtcMeshComponent4->SetMaterial(0, DamagedMaterial);
            EtcMeshComponent5->SetMaterial(0, DamagedMaterial);

        }
        break;

    case EMaterialState::EMS_HandHighLight:

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

        break;

    case EMaterialState::EMS_Death:
        break;

    default:
        break;
    }
}


void AMyXR_CharacterDeffenceBattle::InteractableEffectStart_Implementation()
{
    Super::InteractableEffectStart_Implementation();

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
    ChangeMaterialState(EMaterialState::EMS_OnBoardHighLight, true);
}

void AMyXR_CharacterDeffenceBattle::BuffableEffectEnd_Implementation()
{
    ChangeMaterialState(EMaterialState::EMS_OnBoardHighLight,false);
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

void AMyXR_CharacterDeffenceBattle::FindNearbyEnemy(AXR_Character*& outFirstNear, AXR_Character*& outSecondNear)
{
    Super::FindNearbyEnemy(outFirstNear, outSecondNear);

    TArray<AActor*> AllCharacters;

    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AMyXR_CharacterOffenceBattle::StaticClass(), AllCharacters);

    TArray<AXR_Character*> NearbyCharacters;

    for (AActor* Actor : AllCharacters)
    {
        if (Actor && Actor != this)
        {
            AXR_Character* xrChar = Cast<AXR_Character>(Actor);

            if (xrChar)
            {
                FString ActorName = xrChar->GetName();
                if(CharacterType == ECharacterType::ECT_DefenceT_Arrow_1) UE_LOG(LogTemp, Warning, TEXT("Checking actor: %s"), *ActorName);
                

                if (IHandInteractInterface::Execute_IsOnBoard(xrChar))
                {
                    float Distance = FVector::Dist2D(GetActorLocation(), Actor->GetActorLocation());
                    if (CharacterType == ECharacterType::ECT_DefenceT_Arrow_1) UE_LOG(LogTemp, Warning, TEXT("Distance to actor %s: %f , CharacterProperty.Util_Range : %f"), *ActorName, Distance , CharacterProperty.Util_Range);

                    if (Distance <= CharacterProperty.Util_Range + 3)
                    {
                        NearbyCharacters.Add(xrChar);
                    }
                }
            }
        }
    }

    NearbyCharacters.Sort([this](const AXR_Character& A, const AXR_Character& B)
        {
            return FVector::Dist2D(this->GetActorLocation(), A.GetActorLocation()) < FVector::Dist2D(this->GetActorLocation(), B.GetActorLocation());
        });

    outFirstNear = (NearbyCharacters.Num() > 0) ? NearbyCharacters[0] : nullptr;
    outSecondNear = (NearbyCharacters.Num() > 1) ? NearbyCharacters[1] : nullptr;

    if (outFirstNear)
    {
        UE_LOG(LogTemp, Warning, TEXT("Nearest character: %s"), *outFirstNear->GetName());
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("No nearby characters found"));
    }

    if (outSecondNear)
    {
        UE_LOG(LogTemp, Warning, TEXT("Second nearest character: %s"), *outSecondNear->GetName());
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Only one nearby character found"));
    }
}

void AMyXR_CharacterDeffenceBattle::TargetDieCallBack(AXR_Character* DieTarget)
{
    Super::TargetDieCallBack(DieTarget);

    RenewTargetCharacter12();
}

void AMyXR_CharacterDeffenceBattle::FireBullet(bool isDouble)
{
    if (isDouble && !TargetCharacter2) return;

    FName SockeName = isDouble ? FName("MuzzleSocket2") : FName("MuzzleSocket");



    USkeletalMeshComponent* GunMesh;

    if (isDouble && (CharacterType == ECharacterType::ECT_DefenceT_Arrow_1 || CharacterType == ECharacterType::ECT_DefenceT_Arrow_2))
    {
        SockeName = FName("MuzzleSocket");
        GunMesh = GunMeshComponent2;

        UE_LOG(LogTemp, Display, TEXT("DoubleAttack"));

    }
    else
    {
        GunMesh = GunMeshComponent;
    }

    const USkeletalMeshSocket* MuzzleSocket = GunMesh->GetSocketByName(SockeName);
    AXR_Character* TempChar = isDouble ? TargetCharacter2 : TargetCharacter;

    if (MuzzleSocket && TempChar)
    {
        FTransform MuzzleTransform = MuzzleSocket->GetSocketTransform(GunMesh);

        if (bRangeAttack)
        {
            FVector StartLocation = MuzzleTransform.GetLocation();
            FVector EndLocation = TempChar->GetRingPosition();

            FVector Direction = (EndLocation - StartLocation).GetSafeNormal();
            FRotator ProjectileRotation = Direction.Rotation();

            AProjectile* Projectile = GetWorld()->SpawnActor<AProjectile>(BulletClass, StartLocation, ProjectileRotation);

            if (Projectile)
            {
                Projectile->SetDamage(CharacterProperty.Damage);
                Projectile->SetTarget(EndLocation);
            }
        }
        else
        {
            FVector EndPosition = TempChar->GetActorLocation();

            FHitResult BulletScan;
            GetWorld()->LineTraceSingleByChannel(BulletScan, MuzzleTransform.GetLocation(), EndPosition, ECC_Bullet);

            if (BulletScan.bBlockingHit)
            {
                EndPosition = BulletScan.ImpactPoint;
            }

            UGameplayStatics::ApplyDamage(TempChar, CharacterProperty.Damage, GetController(), this, nullptr);

            if (trailBeam)
            {
                UNiagaraComponent* beamTrailNiagara = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), trailBeam, MuzzleTransform.GetLocation(), FRotator::ZeroRotator, FVector(1.0f), true);

                if (beamTrailNiagara)
                {
                    beamTrailNiagara->SetVariableVec3(FName("TrailStart"), MuzzleTransform.GetLocation());
                    beamTrailNiagara->SetVariableVec3(FName("TrailEnd"), EndPosition);
                }
            }
        }

 
        if (shootParticle)
        {
            UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), shootParticle, MuzzleTransform.GetLocation(), FRotator::ZeroRotator, FVector(1.0f), true);
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

        if (GunMeshComponent2->GetSkeletalMeshAsset() && GunMeshComponent2->GetAnimInstance() && TargetCharacter2)
        {
            GunMeshComponent2->GetAnimInstance()->Montage_Play(GunFireMontage2);

        }
    }



}

void AMyXR_CharacterDeffenceBattle::OnSphereOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    Super::OnSphereOverlapBegin( OverlappedComp,  OtherActor,  OtherComp,  OtherBodyIndex,  bFromSweep, SweepResult);

    if (OtherActor && (OtherActor != this) && OtherComp && Cast<AMyXR_CharacterOffenceBattle>(OtherActor))
    {
        RenewTargetCharacter12();
    }
}

void AMyXR_CharacterDeffenceBattle::RenewTargetCharacter12()
{
    AXR_Character* tempNearest1;
    AXR_Character* tempNearest2;
    FindNearbyEnemy(tempNearest1, tempNearest2);


    if (TargetCharacter && TargetCharacter2)
    {
        // No need to Renew
    }
    else if (TargetCharacter && !TargetCharacter2)
    {
        if (tempNearest1 == TargetCharacter)
        {
            TargetCharacter2 = tempNearest2;
        }
        else
        {
            TargetCharacter2 = tempNearest1;
        }


    }
    else if (!TargetCharacter && TargetCharacter2)
    {
        // Cannot Be
    }
    else
    {
        TargetCharacter = tempNearest1;
        TargetCharacter2 = tempNearest2;
    }
}


