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
    EOT_OffenceGold,
    EOT_DeffenceGold,
    EOT_None
};

UENUM(BlueprintType)
enum class EAnimationState : uint8
{
    EAS_IdleAndWalk,
    EAS_Action,
    EAS_Death,
    EAS_None
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
    ECT_OffenceL,
    ECT_Gold,
    ECT_DefenceP,
    ECT_DefenceT,
    ECT_DefenceT_Laser_1,
    ECT_DefenceT_Laser_2,
    ECT_DefenceT_Canon_1,
    ECT_DefenceT_Canon_2,
    ECT_DefenceT_Fire_1,
    ECT_DefenceT_Fire_2,
    ECT_DefenceT_Arrow_1,
    ECT_DefenceT_Arrow_2,
    ECT_DefenceH,
    ECT_DefenceF1,
    ECT_DefenceF2,

    ECT_None
};


class XRDEFENCEGAME_API XRDefenceEnums
{
public:
	XRDefenceEnums();
	~XRDefenceEnums();
};
