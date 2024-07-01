// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interface/HandInteractInterface.h"
#include "XRDefenceEnums.h"
#include "Components/TimelineComponent.h"
#include "XR_Character.generated.h"

class UNiagaraComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnSetBoardEvent,EObjectType, objectType , ECharacterType, characterType, int32 , SpawnPlaceIndex);


UCLASS()
class XRDEFENCEGAME_API AXR_Character : public ACharacter, public IHandInteractInterface
{
	GENERATED_BODY()

public:
	AXR_Character();

	virtual void Tick(float DeltaTime) override;

	virtual void InteractableEffectStart_Implementation() override;
	virtual void InteractableEffectEnd_Implementation() override;
	virtual void InteractStart_Implementation() override;
	virtual void InteractEnd_Implementation() override;
	virtual void SetInteractPosition_Implementation(FVector GrabPosition) override;

	
	virtual void GrabStart_Implementation() override;
	virtual void GrabEnd_Implementation() override;
	virtual bool IsOnBoard_Implementation() override;

	// Event that invoke when character set on Board
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnSetBoardEvent OnSetBoardEvent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Debug Parameter")
	bool bOnBoard = false;

	UFUNCTION(BlueprintCallable)
	void CheckNeutralToConvert(EObjectType objectType);


protected:
	virtual void BeginPlay() override;

	virtual void PostInitializeComponents() override;

	virtual void InitializeCharacter();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Vital Parameter")
	EObjectType ObjectType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Vital Parameter")
	ECharacterType CharacterType;


	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TObjectPtr<UNiagaraComponent> FromPaletteToCharacter;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TObjectPtr<UNiagaraComponent> FromCharacterToRing;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TObjectPtr <class UFloorRingSMC> FloorRingMesh;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TObjectPtr<class AXRGamePlayMode> XRGamePlayMode;


	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Palette")
	bool bPalletteBeamAvailable;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Palette")
	FVector PalletteBeamEndPosition;

	UPROPERTY(EditDefaultsOnly, Category = "HighLight Parameter")
	UMaterialInstance* HighlightMaterial;

	UPROPERTY(EditDefaultsOnly, Category = "HighLight Parameter")
	float rescaleAmount = 1.1f;

	UPROPERTY(VisibleAnywhere, Category = "Debug Parameter")
	bool bHightLighting = false;

	int32 SpawnPlaceIndex;


	//TimeLIne
	UPROPERTY(VisibleAnywhere)
	class UTimelineComponent* TimelineComponent;

	FOnTimelineFloat InterpFunction;

	UPROPERTY(EditAnywhere, Category = "Dissolve Parameter")
	UCurveFloat* DissolveCurve;
	UFUNCTION()
	virtual void DissolveCallBack(float percent);
	//TimeLIne

	virtual void BindDissolveCallBack();


private:

	//  =================================== Pool ====================================================== 
	bool bPool = true;

	FTransform PoolPlacedTransform;

	//  =================================== Pool ====================================================== 

	void SetRingProperty();




	UPROPERTY()
	class UCharacterMovementComponent* CharacterMovementComponent;


	UPROPERTY()
	USkeletalMeshComponent* CharacterMesh;
	bool GetCharacterMesh();

	UPROPERTY(VisibleAnywhere, Category = "HighLight Parameter")
	UMaterialInstance* DefaultSkeletalMaterialFirst;
	UPROPERTY(VisibleAnywhere, Category = "HighLight Parameter")
	UMaterialInstance* DefaultSkeletalMaterialSecond;

	UPROPERTY(EditDefaultsOnly, Category = "Material Parameter")
	UMaterialInstance* OffenceRingMaterial;
	UPROPERTY(EditDefaultsOnly, Category = "Material Parameter")
	UMaterialInstance* DefenceRingMaterial;
	UPROPERTY(EditDefaultsOnly, Category = "Material Parameter")
	UMaterialInstance* DefaultRingMaterial;

public:
	UFUNCTION(BlueprintCallable)
	void SetSpawnPlaceIndex(int32 index) { SpawnPlaceIndex = index; }

	UFUNCTION(BlueprintCallable)
	bool GetPool() { return bPool; }

	UFUNCTION(BlueprintCallable)
	void SetPool(bool b) { bPool = b; }


};
