// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/XR_Character.h"

AXR_Character::AXR_Character()
{
	PrimaryActorTick.bCanEverTick = true;

}

void AXR_Character::BeginPlay()
{
	Super::BeginPlay();

	if (!GetCharacterMesh()) return;

	CharacterMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	DefaultMaterial = Cast<UMaterialInstance>(CharacterMesh->GetMaterial(0));
}

bool AXR_Character::GetCharacterMesh()
{
	CharacterMesh = (CharacterMesh == nullptr) ? GetMesh() : CharacterMesh;

	if (CharacterMesh == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("AXR_Character::GetCharacterMesh : CharacterMesh == nullptr"));
		return false;
	}

	return true;
}

void AXR_Character::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AXR_Character::InteractableEffectStart_Implementation()
{
	if (!GetCharacterMesh()) return;
	if (bHightLighting) return;

	bHightLighting = true;

	if(HighlightMaterial) CharacterMesh->SetMaterial(0, HighlightMaterial);

	FVector NewScale = CharacterMesh->GetRelativeScale3D() * rescaleAmount; // 10% 더 크게
	CharacterMesh->SetRelativeScale3D(NewScale);


}

void AXR_Character::InteractableEffectEnd_Implementation()
{
	if (!GetCharacterMesh()) return;
	if (!bHightLighting) return;

	bHightLighting = false;

	if (DefaultMaterial) CharacterMesh->SetMaterial(0, DefaultMaterial);

	FVector NewScale = CharacterMesh->GetRelativeScale3D() / rescaleAmount; // 원상복구
	CharacterMesh->SetRelativeScale3D(NewScale);


}

void AXR_Character::InteractStart_Implementation()
{
}

void AXR_Character::InteractEnd_Implementation()
{
}

