// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "XRDefenceEnums.h"
#include "PoolManager.generated.h"

UCLASS()
class XRDEFENCEGAME_API APoolManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APoolManager();

	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

public:	

};
