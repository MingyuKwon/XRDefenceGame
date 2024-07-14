// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/XR_Character.h"
#include "WallNexus.generated.h"

UCLASS()
class XRDEFENCEGAME_API AWallNexus : public AXR_Character
{
	GENERATED_BODY()
	
public:	
	AWallNexus();

protected:
	virtual void BeginPlay() override;

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



public:	
	virtual void Tick(float DeltaTime) override;

};
