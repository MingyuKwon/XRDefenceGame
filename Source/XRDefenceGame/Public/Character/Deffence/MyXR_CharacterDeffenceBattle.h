// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/Deffence/XR_CharacterDeffence.h"
#include "MyXR_CharacterDeffenceBattle.generated.h"

/**
 * 
 */
UCLASS()
class XRDEFENCEGAME_API AMyXR_CharacterDeffenceBattle : public AXR_CharacterDeffence
{
	GENERATED_BODY()

public:
	AMyXR_CharacterDeffenceBattle();
	
    virtual void InteractableEffectStart_Implementation() override;
    virtual void InteractableEffectEnd_Implementation() override;

protected:
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

    virtual void InitializeCharacter() override;  // override 키워드 사용

    UFUNCTION()
    void BindDissolveCallBack() override;

    virtual void DissolveCallBack(float percent) override;


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
