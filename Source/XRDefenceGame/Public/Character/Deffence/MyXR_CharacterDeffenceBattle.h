// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/Deffence/XR_CharacterDeffence.h"
#include "Interface/BuffableInterface.h"
#include "MyXR_CharacterDeffenceBattle.generated.h"

/**
 * 
 */
UCLASS()
class XRDEFENCEGAME_API AMyXR_CharacterDeffenceBattle : public AXR_CharacterDeffence, public IBuffableInterface
{
	GENERATED_BODY()

public:
	AMyXR_CharacterDeffenceBattle();
	
    virtual void InteractableEffectStart_Implementation() override;
    virtual void InteractableEffectEnd_Implementation() override;

    virtual void BuffableEffectStart_Implementation() override;
    virtual void BuffableEffectEnd_Implementation() override;
    virtual void BuffApplied_Implementation(ECharacterType buffType) override;
    virtual int32 GetUpgradeLevel_Implementation() override;
    virtual int32 GetTotalLevel_Implementation() override;

    virtual void NonPalletteSpawnInitalize(FCharacterValueTransmitForm inheritform) override;

    

protected:
    virtual void PackCharacterValueTransmitForm(FCharacterValueTransmitForm& outForm) override;

    virtual void UpdateCharacterPropertyUI() override;


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

    virtual void InitializeCharacter() override;  // override Ű���� ���

    UFUNCTION()
    void BindDissolveCallBack() override;

    virtual void DissolveCallBack(float percent) override;

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

};
