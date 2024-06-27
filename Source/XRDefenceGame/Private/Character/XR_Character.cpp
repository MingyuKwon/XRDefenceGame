// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/XR_Character.h"
#include "NiagaraComponent.h"
#include "Component/FloorRingSMC.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialInstance.h"
#include "XRDefenceGame/XRDefenceGame.h"
#include "Mode/XRGamePlayMode.h"

AXR_Character::AXR_Character()
{
	PrimaryActorTick.bCanEverTick = true;

	FromPaletteToCharacter = CreateDefaultSubobject<UNiagaraComponent>(FName("FromPaletteToCharacter"));
	FromPaletteToCharacter->SetupAttachment(RootComponent);

	FromCharacterToRing = CreateDefaultSubobject<UNiagaraComponent>(FName("FromCharacterToRing"));
	FromCharacterToRing->SetupAttachment(RootComponent);

	FloorRingMesh = CreateDefaultSubobject<UFloorRingSMC>(FName("FloorRingMesh"));
	FloorRingMesh->SetupAttachment(RootComponent);

}

void AXR_Character::BeginPlay()
{
	Super::BeginPlay();

	if (!GetCharacterMesh()) return;

	CharacterMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	DefaultMaterial = Cast<UMaterialInstance>(CharacterMesh->GetMaterial(0));


	XRGamePlayMode = Cast<AXRGamePlayMode>(UGameplayStatics::GetGameMode(this));

	bPalletteBeamAvailable = false;
	FromPaletteToCharacter->SetVisibility(false);
	FromCharacterToRing->SetVisibility(false);

	switch (ObjectType)
	{
		case EObjectType::EOT_Offence:
			FloorRingMesh->SetMaterial(0, OffenceRingMaterial);
			FloorRingMesh->beneathTraceChannel = ECC_AttackBoard;
			FloorRingMesh->SetMaterialCall();
			break;
		case EObjectType::EOT_Deffence:
			FloorRingMesh->SetMaterial(0, DefenceRingMaterial);
			FloorRingMesh->beneathTraceChannel = ECC_DefenceBoard;
			FloorRingMesh->SetMaterialCall();
			break;
		case EObjectType::EOT_Neutral:
			FloorRingMesh->SetMaterial(0, DefaultRingMaterial);
			FloorRingMesh->beneathTraceChannel = ECC_Board;
			FloorRingMesh->SetMaterialCall();
			break;
		case EObjectType::EOT_None:
			break;
		default:
			break;
	}

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

	FromCharacterToRing->SetVectorParameter("User.BeamEnd", FloorRingMesh->GetComponentLocation());
}

void AXR_Character::InteractableEffectStart_Implementation()
{
	if (!GetCharacterMesh()) return;
	if (bHightLighting) return;

	bHightLighting = true;

	if(HighlightMaterial) CharacterMesh->SetMaterial(0, HighlightMaterial);

	FVector NewScale = CharacterMesh->GetRelativeScale3D() * rescaleAmount; 
	CharacterMesh->SetRelativeScale3D(NewScale);


}

void AXR_Character::InteractableEffectEnd_Implementation()
{
	if (!GetCharacterMesh()) return;
	if (!bHightLighting) return;

	bHightLighting = false;

	if (DefaultMaterial) CharacterMesh->SetMaterial(0, DefaultMaterial);

	FVector NewScale = CharacterMesh->GetRelativeScale3D() / rescaleAmount; 
	CharacterMesh->SetRelativeScale3D(NewScale);


}

void AXR_Character::InteractStart_Implementation()
{

}

void AXR_Character::InteractEnd_Implementation()
{

}

void AXR_Character::SetInteractPosition_Implementation(FVector GrabPosition)
{
	SetActorLocation(GrabPosition);
}

void AXR_Character::GrabStart_Implementation()
{
	// Execute in Blueprint
}

void AXR_Character::GrabEnd_Implementation()
{
	// Execute in Blueprint
}

bool AXR_Character::IsOnBoard_Implementation()
{
	return bOnBoard;
}

