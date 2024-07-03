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

    virtual void Tick(float DeltaTime) override;


protected:
    virtual void BeginPlay() override;

    virtual void BindDissolveCallBack() override;

    virtual void DissolveCallBack(float percent) override;

    virtual void DissolveCallBackReverse(float percent) override;

    virtual void SetPalletteCharacterOnBoard(bool isOnBoard, AXR_Character* beneathBuffableCharacter) override;


    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UStaticMeshComponent* RingMeshComponent1;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UStaticMeshComponent* RingMeshComponent2;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UStaticMeshComponent* RingMeshComponent3;

    virtual void OnBoardCalledFunction(bool isOnBoard) override;

    AXR_Character* BeneathBuffableCharacter;

private:
    float ElapsedTime;
    float MinZ;
    float MaxZ;

    FVector InitialLocation1;
    FVector InitialLocation2;
    FVector InitialLocation3;

    bool bMovingUp1;
    bool bMovingUp2;
    bool bMovingUp3;

    void UpdateComponentPosition(USceneComponent* Component, FVector InitialLocation, bool& bMovingUp, float DeltaTime, float MoveSpeed);

    void LifeTimeTimerFunction();

    FTimerHandle LifeTimeTimerHandle;


};
