// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/Deffence/XR_CharacterDeffence.h"
#include "MyXR_CharacterDeffenceBuff.generated.h"

/**
 * 
 */
UCLASS()
class XRDEFENCEGAME_API AMyXR_CharacterDeffenceBuff : public AXR_CharacterDeffence
{
	GENERATED_BODY()

public:
    AMyXR_CharacterDeffenceBuff();

protected:
    virtual void BeginPlay() override;


    UFUNCTION()
    void BindDissolveCallBack() override;

    virtual void DissolveCallBack(float percent) override;



    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UStaticMeshComponent* RingMeshComponent1;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UStaticMeshComponent* RingMeshComponent2;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UStaticMeshComponent* RingMeshComponent3;
	
};
