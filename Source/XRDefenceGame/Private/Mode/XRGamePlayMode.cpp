// Fill out your copyright notice in the Description page of Project Settings.


#include "Mode/XRGamePlayMode.h"

void AXRGamePlayMode::TriggerOnObjectGrabEvent(bool isGrab, EObjectType objectType)
{
	OnObjectGrabEvent.Broadcast(isGrab, objectType);

}
