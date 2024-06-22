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


class XRDEFENCEGAME_API XRDefenceEnums
{
public:
	XRDefenceEnums();
	~XRDefenceEnums();
};
