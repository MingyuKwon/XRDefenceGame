// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/XR_Character.h"
#include "XRDefenceEnums.h"
#include "WallNexus.generated.h"

class UMaterialInstance;

UCLASS()
class XRDEFENCEGAME_API AWallNexus : public AXR_Character
{
	GENERATED_BODY()
	
public:	
	AWallNexus();

protected:
	virtual void BeginPlay() override;

	virtual void InitializeCharacter() override;


	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> NexusMesh1;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> NexusMesh2;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> NexusMesh3;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> NexusMesh4;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> NexusMesh5;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TObjectPtr<class UBoxComponent> RootCollision;


	UPROPERTY(EditDefaultsOnly, Category = "Nexus Parameter")
	ENexusType nexusType;

	UPROPERTY(VisibleAnywhere, Category = "HighLight Parameter")
	UMaterialInstance* DefaultNexusMesh1;

	UPROPERTY(VisibleAnywhere, Category = "HighLight Parameter")
	UMaterialInstance* DefaultNexusMesh2;

	UPROPERTY(VisibleAnywhere, Category = "HighLight Parameter")
	UMaterialInstance* DefaultNexusMesh3;

	UPROPERTY(VisibleAnywhere, Category = "HighLight Parameter")
	UMaterialInstance* DefaultNexusMesh4;

	UPROPERTY(VisibleAnywhere, Category = "HighLight Parameter")
	UMaterialInstance* DefaultNexusMesh5;

	virtual void DissolveCallBackReverse(float percent) override;
	virtual void DissolveCallBack(float percent) override;

	virtual void ChangeMaterialEMS_Default() override;
	virtual void ChangeMaterialEMS_OnBoardHighLight() override;
	virtual void ChangeMaterialEMS_Damage() override;
	virtual void ChangeMaterialEMS_HandHighLight() override;
	virtual void ChangeMaterialEMS_Death() override;


	float TimeAccumulator;
	bool bMovingUp;

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

public:	
	virtual void Tick(float DeltaTime) override;

};
