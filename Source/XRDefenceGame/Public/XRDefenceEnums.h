// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"


UENUM(BlueprintType)
enum class Pose : uint8
{
    Grab,
    Rock,
    scissors,
    Paper,
    Thumb,
    None
};

UENUM(BlueprintType)
enum class EObjectType : uint8
{
    EOT_Offence,
    EOT_Deffence,
    EOT_Neutral,
    EOT_None
};


UENUM(BlueprintType)
enum class ECharacterType : uint8
{
    ECT_OffenceD,
    ECT_OffenceDT,
    ECT_OffenceT,
    ECT_OffenceH,
    ECT_OffenceB,
    ECT_OffenceS,
    ECT_Gold,
    ECT_DefenceP,
    ECT_DefenceT,
    ECT_None
};


class XRDEFENCEGAME_API XRDefenceEnums
{
public:
	XRDefenceEnums();
	~XRDefenceEnums();
};
