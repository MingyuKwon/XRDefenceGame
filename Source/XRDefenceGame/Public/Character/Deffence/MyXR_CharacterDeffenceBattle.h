// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/Deffence/XR_CharacterDeffence.h"
#include "Interface/BuffableInterface.h"
#include "MyXR_CharacterDeffenceBattle.generated.h"

class UAnimMontage;
/**
 * 
 */
UCLASS()
class XRDEFENCEGAME_API AMyXR_CharacterDeffenceBattle : public AXR_CharacterDeffence, public IBuffableInterface
{
	GENERATED_BODY()

public:
    virtual void Tick(float DeltaTime) override;

	AMyXR_CharacterDeffenceBattle();
	
    virtual void InteractableEffectStart_Implementation() override;
    virtual void InteractableEffectEnd_Implementation() override;

    virtual void BuffableEffectStart_Implementation() override;
    virtual void BuffableEffectEnd_Implementation() override;
    virtual void BuffApplied_Implementation(ECharacterType buffType) override;
    virtual int32 GetUpgradeLevel_Implementation() override;
    virtual int32 GetTotalLevel_Implementation() override;

    virtual void NonPalletteSpawnInitalize(FCharacterValueTransmitForm inheritform) override;

    virtual void CharacterActionImpact() override;
    virtual void CharacterActionImpact2() override;

    virtual void FindNearbyEnemy(AXR_Character*& outFirstNear, AXR_Character*& outSecondNear) override;

protected:

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Niagara Parameter")
    class UNiagaraSystem* trailBeam;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Niagara Parameter")
    UNiagaraSystem* shootParticle;

    virtual void TargetDieCallBack(AXR_Character* DieTarget) override;


    virtual void FireBullet(bool isDouble = false);

    virtual void PackCharacterValueTransmitForm(FCharacterValueTransmitForm& outForm) override;

    virtual void UpdateCharacterPropertyUI() override;

    UPROPERTY(EditAnywhere, Category = "Anim Parameter")
    UAnimMontage* GunFireMontage = nullptr;

    UPROPERTY(EditAnywhere, Category = "Anim Parameter")
    UAnimMontage* GunFireMontage2 = nullptr;


    UPROPERTY(EditAnywhere, Category = "Anim Parameter")
    UAnimMontage* GunSetStartFireMontage = nullptr;

    UPROPERTY(EditAnywhere, Category = "Anim Parameter")
    UAnimMontage* GunSetEndFireMontage = nullptr;

    virtual void CharacterActionStart() override;

    virtual void OnSphereOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

    void RenewTargetCharacter12();


    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    USkeletalMeshComponent* GunMeshComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    USkeletalMeshComponent* GunMeshComponent2;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UStaticMeshComponent* EtcMeshComponent1;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UStaticMeshComponent* EtcMeshComponent2;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UStaticMeshComponent* EtcMeshComponent3;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UStaticMeshComponent* EtcMeshComponent4;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UStaticMeshComponent* EtcMeshComponent5;

    virtual void InitializeCharacter() override;  

    UFUNCTION()
    void BindDissolveCallBack() override;

    virtual void DissolveCallBack(float percent) override;

    virtual void DissolveCallBackReverse(float percent) override;


    UPROPERTY(EditAnywhere, Category = "Battle Parameter")
    TSubclassOf<class AProjectile> BulletClass;

    UPROPERTY(EditAnywhere, Category = "Battle Parameter")
    bool bAttackBoth = false;

    UPROPERTY(EditAnywhere, Category = "Battle Parameter")
    bool bRangeAttack = false;


    UPROPERTY(VisibleAnywhere, Category = "Debug Parameter")
    bool bBufferHightLighting = false;

    virtual void HighLightMesh(bool bHighlight) override;

    UPROPERTY(VisibleAnywhere, Category = "Buff Parameter")
    int32 DamageUpgradeCount = 0;

    UPROPERTY(VisibleAnywhere, Category = "Buff Parameter")
    int32 RangeUpgradeCount = 0;

    void UpgradeTurret(ECharacterType characterType);

private:
    UPROPERTY(VisibleAnywhere, Category = "HighLight Parameter")
    UMaterialInstance* DefaultGunMaterial;

    UPROPERTY(VisibleAnywhere, Category = "HighLight Parameter")
    UMaterialInstance* DefaultGun2Material;

    UPROPERTY(VisibleAnywhere, Category = "HighLight Parameter")
    UMaterialInstance* DefaultEtcMaterialFirst;

    UPROPERTY(VisibleAnywhere, Category = "HighLight Parameter")
    UMaterialInstance* DefaultEtcMaterialSecond;

    UPROPERTY(VisibleAnywhere, Category = "HighLight Parameter")
    UMaterialInstance* DefaultEtcMaterialThird;

    UPROPERTY(VisibleAnywhere, Category = "HighLight Parameter")
    UMaterialInstance* DefaultEtcMaterialForth;

    UPROPERTY(VisibleAnywhere, Category = "HighLight Parameter")
    UMaterialInstance* DefaultEtcMaterialFifth;


    FRotator DefaultTargetRotation;
    FRotator TargetRotation;

};
