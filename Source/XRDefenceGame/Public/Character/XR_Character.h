// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interface/HandInteractInterface.h"
#include "XRDefenceEnums.h"

#include "XR_Character.generated.h"

class UNiagaraComponent;

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

	
	  
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Debug Parameter")
	bool bOnBoard = false;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Vital Parameter")
	EObjectType ObjectType;


	/** Please add a variable description */
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Niagara Parameter")
	TObjectPtr<UNiagaraComponent> FromPaletteToCharacter;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Niagara Parameter")
	TObjectPtr<UNiagaraComponent> FromCharacterToRing;



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
