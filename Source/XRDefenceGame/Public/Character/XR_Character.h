// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interface/HandInteractInterface.h"

#include "XR_Character.generated.h"

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



protected:
	virtual void BeginPlay() override;


private:

	UPROPERTY()
	USkeletalMeshComponent* CharacterMesh;
	bool GetCharacterMesh();

	UPROPERTY(EditDefaultsOnly, Category = "HighLight Parameter")
	UMaterialInstance* HighlightMaterial;
	UPROPERTY(VisibleAnywhere, Category = "HighLight Parameter")
	UMaterialInstance* DefaultMaterial;

	UPROPERTY(EditDefaultsOnly, Category = "HighLight Parameter")
	float rescaleAmount = 1.1f;


	UPROPERTY(VisibleAnywhere, Category = "Debug Parameter")
	bool bHightLighting = false;

};
