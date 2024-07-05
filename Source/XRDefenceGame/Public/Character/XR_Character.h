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

USTRUCT(BlueprintType)
struct FCharacterValueTransmitForm
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Property Parameter")
	float currentHealth = 0;


	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Buff Parameter")
	int32 DamageUpgradeCount = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Buff Parameter")
	int32 RangeUpgradeCount = 0;
};

USTRUCT(BlueprintType)
struct FCharacterStatus
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Property Parameter")
	float currentHealth = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Property Parameter")
	float MaxHealth = 20;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Property Parameter")
	float Damage = 3;

	//Only Uses Attacker
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Property Parameter")
	float Util_Fast = 2;
	
	// Use Both
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Property Parameter")
	float Util_Range = 4;

	//Only Uses Defender
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Property Parameter")
	float ObjectAccessRadius = 1;

};

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

	UFUNCTION(BlueprintCallable)
	virtual void NonPalletteSpawnInitalize(FCharacterValueTransmitForm inheritform);

	// Event that invoke when character set on Board
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnSetBoardEvent OnSetBoardEvent;

	UFUNCTION(BlueprintCallable)
	void CheckNeutralToConvert(EObjectType objectType);

	virtual void Heal(float healAmount);


protected:

	UPROPERTY()
	class AXRAIController* XRAIController;

	UPROPERTY(EditAnywhere, Category = "AI Parameter")
	class UBehaviorTree* BehaviorTree = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug Parameter")
	bool DefaultPlaceInBoard = false;

	virtual void DestroyMyself();

	virtual void SetOnBoardAuto();


	virtual void SetPropertyUIVisible(bool flag);


	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Debug Parameter")
	TMap<int32, ECharacterType> TurretTypeMap;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Blueprint parameter")
	TSubclassOf<class ACharacterUI> characterProperyUIClass;

	ACharacterUI* CharacterPropertyUI = nullptr;

	void SpawnCharacterPropertyUI();

	virtual void UpdateCharacterPropertyUI();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug Parameter")
	FCharacterStatus CharacterProperty;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Debug Parameter")
	bool bOnBoard = false;

	virtual void OnBoardCalledFunction(bool isOnBoard, bool isSpawnedByHand);

	virtual void BeginPlay() override;

	virtual void PossessedBy(AController* NewController) override;


	virtual void InitializeCharacter();

	virtual void SetPalletteCharacterOnBoard(bool isOnBoard, AXR_Character* beneathBuffableCharacter);

	virtual void PackCharacterValueTransmitForm(FCharacterValueTransmitForm& outForm);


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

	UPROPERTY(VisibleAnywhere, Category = "HighLight Parameter")
	UMaterialInstance* DefaultSkeletalMaterialFirst;
	UPROPERTY(VisibleAnywhere, Category = "HighLight Parameter")
	UMaterialInstance* DefaultSkeletalMaterialSecond;

	UPROPERTY()
	USkeletalMeshComponent* CharacterMesh;
	bool GetCharacterMesh();


	//TimeLIne
	UPROPERTY(VisibleAnywhere)
	class UTimelineComponent* TimelineComponent;

	FOnTimelineFloat InterpFunction;

	UPROPERTY(EditAnywhere, Category = "Dissolve Parameter")
	UCurveFloat* DissolveCurve;

	UFUNCTION()
	virtual void DissolveCallBack(float percent);

	UFUNCTION()
	virtual void DissolveCallBackReverse(float percent);

	//TimeLIne

	virtual void BindDissolveCallBack();

	virtual void BindReverseDissolveCallBack();


	UFUNCTION()
	virtual void HighLightMesh(bool bHighlight);

	virtual void StartDissolveTimeline(bool bNotReverse);

	virtual void Death();

	UFUNCTION()
	virtual void DeathTimerFunction();

	UFUNCTION()
	virtual void BehaviorAvailableTimerFunction();


	FTimerHandle DeathTimerHandle;

	FTimerHandle BehaviorAvailableTimerHandle;

	bool bBehaviorAvailable = false;

private:

	void SetRingProperty();

	UPROPERTY()
	class UCharacterMovementComponent* CharacterMovementComponent;


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
	EObjectType GetObjectType() { return ObjectType; }

	UFUNCTION(BlueprintCallable)
	ECharacterType GetCharacterType() { return CharacterType; }

	UFUNCTION(BlueprintCallable)
	bool IsBehaviorAvailable() { return bBehaviorAvailable; }

	UFUNCTION(BlueprintCallable)
	float GetAttackRange() { return CharacterProperty.Util_Range; }

	UFUNCTION(BlueprintCallable)
	float GetAccessRadius() { return CharacterProperty.ObjectAccessRadius; }
	
};
