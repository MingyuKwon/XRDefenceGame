// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/NotHitSelf_PMC.h"

UNotHitSelf_PMC::EHandleBlockingHitResult UNotHitSelf_PMC::HandleBlockingHit(const FHitResult& Hit, float TimeTick, const FVector& MoveDelta, float& SubTickTimeRemaining)
{
	Super::HandleBlockingHit(Hit, TimeTick, MoveDelta, SubTickTimeRemaining);
	return EHandleBlockingHitResult::AdvanceNextSubstep;
}

void UNotHitSelf_PMC::HandleImpact(const FHitResult& Hit, float TimeSlice, const FVector& MoveDelta)
{

}