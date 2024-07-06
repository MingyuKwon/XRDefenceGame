// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/Offence/XR_CharacterOffence.h"
#include "MyXR_CharacterOffenceBattle.generated.h"

/**
 * 
 */
UCLASS()
class XRDEFENCEGAME_API AMyXR_CharacterOffenceBattle : public AXR_CharacterOffence
{
	GENERATED_BODY()
	
public:
	virtual void CharacterActionImpact() override;

protected:
	virtual void CharacterActionStart() override;

};
