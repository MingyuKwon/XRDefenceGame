// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interface/HandInteractInterface.h"
#include "XRDefenceEnums.h"
#include "Components/TimelineComponent.h"
#include "XR_Character.generated.h"

class UNiagaraComponent;
class UXRDefenceGameInstance;
class UAudioSubsystem;
class ACostShowChip;
class UAnimMontage;
class UNiagaraSystem;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnSetBoardEvent,EObjectType, objectType , ECharacterType, characterType, int32 , SpawnPlaceIndex);

USTRUCT(BlueprintType)
struct FCharacterValueTransmitForm
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Property Parameter")
	float currentHealth = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Property Parameter")
	float beforeMaxHealth = 0;


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
	float currentDamage = 3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Property Parameter")
	float defaultDamage = 3;


	//Only Uses Attacker
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Property Parameter")
	float Util_Fast = 2;
	
	// Use Both
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Property Parameter")
	float Util_Range = 4;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Property Parameter")
	float RangeAcceptError = 3;


	//Only Uses Defender
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Property Parameter")
	float ObjectAccessRadius = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Property Parameter")
	float Cost = 5;


};

UCLASS()
class XRDEFENCEGAME_API AXR_Character : public ACharacter, public IHandInteractInterface
{
	GENERATED_BODY()

public:
	AXR_Character();

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Network")
	int32 ActorNetID = -1;

	virtual void Tick(float DeltaTime) override;

	virtual void InteractableEffectStart_Implementation() override;

	virtual void InteractableEffectEnd_Implementation() override;

	virtual void InteractStart_Implementation() override;
	virtual void InteractEnd_Implementation() override;

	virtual void SetInteractPosition_Implementation(FVector GrabPosition) override;

	virtual void GrabStart_Implementation() override;

	virtual void GrabEnd_Implementation() override;

	virtual EObjectType GetInteractObjectType_Implementation() override;


	virtual void SetDisableHighLight_Implementation(bool bDisable) override;

	virtual bool IsOnBoard_Implementation() override;
	virtual float GetCost_Implementation() override;
	virtual int32 GetNetId_Implementation() override;
	virtual bool GetDisableHighLight_Implementation() override;



	UFUNCTION(BlueprintCallable, Server, Reliable)
	virtual void Server_InteractableEffectStart();
	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
	virtual void Multi_InteractableEffectStart();

	UFUNCTION(BlueprintCallable, Server, Reliable)
	virtual void Server_InteractableEffectEnd();
	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
	virtual void Multi_InteractableEffectEnd();

	UFUNCTION(BlueprintCallable, Server, Reliable)
	virtual void Server_SetInteractPosition(FVector GrabPosition);
	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
	virtual void Multi_SetInteractPosition(FVector GrabPosition);

	UFUNCTION(BlueprintCallable, Server, Reliable)
	virtual void Server_GrabStart();
	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
	virtual void Multi_GrabStart();

	UFUNCTION(BlueprintCallable, Server, Reliable)
	virtual void Server_GrabEnd();
	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
	virtual void Multi_GrabEnd();

	UFUNCTION(BlueprintCallable, Server, Reliable)
	virtual void Server_SetbDisableInteractable(bool flag);
	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
	virtual void Multi_SetbDisableInteractable(bool flag);


	UFUNCTION(BlueprintCallable)
	virtual void NonPalletteSpawnInitalize(FCharacterValueTransmitForm inheritform);

	UFUNCTION(BlueprintCallable)
	virtual void SetOnBoardAuto();

	// Event that invoke when character set on Board
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnSetBoardEvent OnSetBoardEvent;

	UFUNCTION(BlueprintCallable)
	void CheckNeutralToConvert(EObjectType objectType);

	virtual void Heal(float healAmount);

	virtual void AttackBuff(float BuffAmount);


	UFUNCTION(BlueprintCallable)
	void CharacterActionCall();

	UFUNCTION(BlueprintCallable)
	virtual void CharacterActionImpact();

	UFUNCTION(BlueprintCallable)
	virtual void CharacterActionSound();


	UFUNCTION(BlueprintCallable)
	virtual void CharacterActionImpact2();


	UFUNCTION(BlueprintCallable)
	virtual void CharacterActionEnd();

	UFUNCTION(BlueprintCallable)
	virtual void FindNearbyEnemy(AXR_Character*& outFirstNear, AXR_Character*& outSecondNear);

	UFUNCTION(BlueprintCallable)
	void UpdateMotoionWarpingTransform();

	UFUNCTION(BlueprintCallable)
	virtual void SetTrashEffect(bool flag, bool onlyNiagara = false);

	UFUNCTION(BlueprintCallable)
	virtual void TriggerStun();

	UFUNCTION(BlueprintCallable)
	virtual void TriggerMoveFast();

	UFUNCTION(BlueprintCallable)
	virtual void TriggerMoveSlow();


protected:

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

	UFUNCTION(BlueprintCallable)
	virtual void CallBackForPallette();

	UXRDefenceGameInstance* GameInstance;
	UAudioSubsystem* AudioManager;

	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
	virtual void Multi_PlaySound(EGameSoundType soundType, USoundBase* Sound, FVector Location, float VolumeScale);

	UFUNCTION(BlueprintCallable, Server, Reliable)
	virtual void Server_PlaySound(EGameSoundType soundType, USoundBase* Sound, FVector Location, float VolumeScale);

	virtual void PlaySound(EGameSoundType soundType, USoundBase* Sound, FVector Location, float VolumeScale);

	virtual void PlaySoundViaManager(EGameSoundType soundType, USoundBase* Sound, FVector Location, float VolumeScale, bool bLocal = false);


	UPROPERTY(EditAnywhere, Category = "Sound Parameter")
	float OwnVolumeScale = 1.f;

	UPROPERTY(EditAnywhere, Category = "Sound Parameter")
	USoundBase* SoundSpawnBoard;

	UPROPERTY(EditAnywhere, Category = "Sound Parameter")
	USoundBase* SoundDeath;

	UPROPERTY(EditAnywhere, Category = "Sound Parameter")
	USoundBase* SoundDeathInTrash;

	UPROPERTY(EditAnywhere, Category = "Sound Parameter")
	USoundBase* SoundDamaged;

	UPROPERTY(EditAnywhere, Category = "Sound Parameter")
	USoundBase* SoundSpawnPallette;

	UPROPERTY(EditAnywhere, Category = "Sound Parameter")
	USoundBase* SoundHighLight;

	UPROPERTY(EditAnywhere, Category = "Sound Parameter")
	USoundBase* SoundAction; 



	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
	virtual void ChangeMaterialState(EMaterialState materialState, bool bLock);

	UFUNCTION(BlueprintCallable)
	virtual void ChangeMaterialEMS_Default();

	UFUNCTION(BlueprintCallable)
	virtual void ChangeMaterialEMS_OnBoardHighLight();

	UFUNCTION(BlueprintCallable)
	virtual void ChangeMaterialEMS_Stun();


	UFUNCTION(BlueprintCallable)
	virtual void ChangeMaterialEMS_Damage();

	UFUNCTION(BlueprintCallable)
	virtual void ChangeMaterialEMS_HandHighLight();

	UFUNCTION(BlueprintCallable)
	virtual void ChangeMaterialEMS_Death();

	
	bool bLockOnBoardHighLight = false;
	bool bLockOnStun = false;
	bool bLockDamage = false;
	bool bLockHandHighLight = false;
	bool bLockDeath = false;


	UFUNCTION(BlueprintCallable)
	virtual void CharacterActionStart();



	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION()
	virtual void TargetDieCallBack(AXR_Character* DieTarget);

	UFUNCTION()
	virtual void OtherCharacterSpawnCallBack(FVector spawnLocation);

	UFUNCTION()
	virtual void GameEndCallBack();



	UPROPERTY(VisibleAnywhere, Category = "Debug Parameter" , Replicated)
	AXR_Character* TargetCharacter = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Debug Parameter", Replicated)
	AXR_Character* TargetCharacter2 = nullptr;


	UPROPERTY(VisibleAnywhere, Category = "Debug Parameter", Replicated)
	EAnimationState AnimState = EAnimationState::EAS_IdleAndWalk;

	UFUNCTION(BlueprintCallable)
	void SetAnimState(EAnimationState state);

	UFUNCTION(NetMulticast, Reliable)
	void PlayAnimMontageMulti(USkeletalMeshComponent* skeletalComponent, UAnimMontage* montage);


	UPROPERTY(EditAnywhere, Category = "Anim Parameter")
	UAnimMontage* CharacterActionMontage = nullptr;

	UPROPERTY(EditAnywhere, Category = "Anim Parameter")
	UAnimMontage* CharacterDeathMontage = nullptr;


	UPROPERTY()
	class AXRAIController* XRAIController;

	UPROPERTY(EditAnywhere, Category = "AI Parameter")
	class UBehaviorTree* BehaviorTree = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug Parameter")
	bool DefaultPlaceInBoard = false;

	virtual void DestroyMyself();



	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Debug Parameter")
	TMap<int32, ECharacterType> TurretTypeMap;



	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Blueprint parameter")
	TSubclassOf<class ACharacterUI> characterProperyUIClass;

	ACharacterUI* CharacterPropertyUI = nullptr;

	virtual void SetPropertyUIVisible(bool flag);


	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Blueprint parameter")
	TSubclassOf<class ACostShowChip> costShowUIClass;

	ACostShowChip* CostShowUI = nullptr;


	void SpawnCostShowUI();

	void SpawnCharacterPropertyUI();

	virtual void UpdateCharacterPropertyUI();


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug Parameter",  Replicated)
	FCharacterStatus CharacterProperty;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Debug Parameter", Replicated)
	bool bOnBoard = false;

	virtual void OnBoardCalledFunctionServer(bool isOnBoard, bool isSpawnedByHand);

	virtual void OnBoardCalledFunction(bool isOnBoard, bool isSpawnedByHand);

	virtual void BeginPlay() override;

	virtual void PossessedBy(AController* NewController) override;


	virtual void InitializeCharacter();

	virtual void SetPalletteCharacterOnBoard(bool isOnBoard, AXR_Character* beneathBuffableCharacter);

	virtual void PackCharacterValueTransmitForm(FCharacterValueTransmitForm& outForm);


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vital Parameter", Replicated)
	EObjectType ObjectType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vital Parameter")
	ECharacterType CharacterType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vital Parameter")
	float StunTime = 2.f;


	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TObjectPtr<UNiagaraComponent> DeathCilinderCharacter;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TObjectPtr<UNiagaraComponent> SpawnCilinderCharacter;


	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TObjectPtr<UNiagaraComponent> FromPaletteToCharacter;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TObjectPtr<UNiagaraComponent> FromCharacterToRing;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TObjectPtr<UNiagaraComponent> BuffRing;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TObjectPtr<UNiagaraComponent> HealRing;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TObjectPtr<UNiagaraComponent> SpeedBuffNiagara;


	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	class UMotionWarpingComponent* MotionWarpingComponent;



	UFUNCTION(NetMulticast, Reliable)
	void TriggerHealEffect();

	UFUNCTION(NetMulticast, Reliable)
	void TriggerBuffEffect();


	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TObjectPtr <class UFloorRingSMC> FloorRingMesh;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TObjectPtr<class AXRGamePlayMode> XRGamePlayMode;


	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Palette")
	bool bPalletteBeamAvailable;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Palette")
	FVector PalletteBeamEndPosition;

	UPROPERTY(EditDefaultsOnly, Category = "Material Parameter")
	UMaterialInstance* DamagedMaterial;

	UPROPERTY(EditDefaultsOnly, Category = "HighLight Parameter")
	UMaterialInstance* HighlightMaterial;

	UPROPERTY(EditDefaultsOnly, Category = "HighLight Parameter")
	UMaterialInstance* StunMaterial;


	UPROPERTY(EditDefaultsOnly, Category = "HighLight Parameter")
	UMaterialInstance* DisableHighlightMaterial;

	UPROPERTY(VisibleAnywhere, Category = "Debug Parameter", Replicated)
	bool bDisableInteractable = false;


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

	UPROPERTY(VisibleAnywhere)
	class USphereComponent* sphereOverlapCheck;

	UFUNCTION()
	virtual void OnSphereOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);


	FOnTimelineFloat InterpFunction;

	UPROPERTY(EditAnywhere, Category = "Dissolve Parameter")
	UCurveFloat* DissolveCurve;

	UFUNCTION(NetMulticast, Reliable)
	virtual void DissolveCallBackMulti(float percent);

	UFUNCTION(NetMulticast, Reliable)
	virtual void DissolveCallBackReverseMulti(float percent);


	virtual void DissolveCallBack(float percent);

	virtual void DissolveCallBackReverse(float percent);

	//TimeLIne

	virtual void StartDissolveTimeline(bool bNotReverse);

	virtual void Death(bool bDieInTrash);



	UFUNCTION()
	virtual void DeathTimerFunction();

	UFUNCTION()
	virtual void BehaviorAvailableTimerFunction();

	virtual void DamageTimerFunction();

	virtual void DamageStartFunction();

	UFUNCTION()
	virtual void BuffEndTimerFunction();

	UFUNCTION()
	virtual void StunEndTimerFunction();


	UPROPERTY(EditAnywhere, Category = "Buff Parameter")
	float BuffTime = 5.f;

	FTimerHandle BuffTimerHandle;

	FTimerHandle DamageTimerHandle;

	FTimerHandle DeathTimerHandle;

	FTimerHandle BehaviorAvailableTimerHandle;

	FTimerHandle StunTimerHandle;

	FTimerHandle MoveSpeedUpHandle;

	FTimerHandle MoveSpeedDownHandle;

	UPROPERTY(Replicated)
	bool bBehaviorAvailable = false;

	UPROPERTY(Replicated)
	bool bNowStun = false;

	UFUNCTION(NetMulticast, Reliable)
	void MoveSpeedUp(bool bEffectOn);

	UFUNCTION(NetMulticast, Reliable)
	void MoveSpeedDown(bool bEffectOn);

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

	UFUNCTION(BlueprintCallable)
	AXR_Character* GetTargetCharacter() { return TargetCharacter; }

	UFUNCTION(BlueprintCallable)
	AXR_Character* GetTargetCharacter2() { return TargetCharacter2; }


	UFUNCTION(BlueprintCallable)
	void SetTargetCharacter(AXR_Character* target) { TargetCharacter = target; }

	UFUNCTION(BlueprintCallable)
	void SetTargetCharacter2(AXR_Character* target) { TargetCharacter2 = target; }

	UFUNCTION(BlueprintCallable)
	FVector GetRingPosition();

	UFUNCTION(BlueprintCallable)
	bool GebPalletteBeamAvailable() { return bPalletteBeamAvailable; }

	UFUNCTION(BlueprintCallable)
	bool GetbDisableInteractable() { return bDisableInteractable; }

	UFUNCTION(BlueprintCallable)
	bool GetbIsNowStun() { return bNowStun; }

	UFUNCTION(BlueprintCallable)
	bool GetIsNowCanWalkBasedOnAnimation() { return AnimState <= EAnimationState::EAS_IdleAndWalk ; }

	

};
