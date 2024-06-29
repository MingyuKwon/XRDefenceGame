// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/XR_Character.h"
#include "NiagaraComponent.h"
#include "Component/FloorRingSMC.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialInstance.h"
#include "XRDefenceGame/XRDefenceGame.h"
#include "GameFramework/CharacterMovementComponent.h"

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

	TimelineComponent = CreateDefaultSubobject<UTimelineComponent>(TEXT("DissolveTimeline"));

	CharacterMovementComponent = GetCharacterMovement();
}

void AXR_Character::BeginPlay()
{
	Super::BeginPlay();

	InitializeCharacter();

}

void AXR_Character::PostInitializeComponents()
{
	Super::PostInitializeComponents();

}


void AXR_Character::InitializeCharacter()
{
	if (!GetCharacterMesh()) return;

	CharacterMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	DefaultMaterialFirst = Cast<UMaterialInstance>(CharacterMesh->GetMaterial(0));
	DefaultMaterialSecond = Cast<UMaterialInstance>(CharacterMesh->GetMaterial(1));

	XRGamePlayMode = Cast<AXRGamePlayMode>(UGameplayStatics::GetGameMode(this));

	if (CharacterMovementComponent)
	{
		CharacterMovementComponent->SetMovementMode(EMovementMode::MOVE_Walking));
		
		FString ActorName = GetName();
		int32 HashValue = FCrc::StrCrc32(*ActorName);
		FString DebugMessage = FString::Printf(TEXT("Actor: %s, Movement Mode: %s, ,Default Movement Mode: %s"),
			*ActorName,
			*UEnum::GetValueAsString(CharacterMovementComponent->MovementMode),
			*UEnum::GetValueAsString(CharacterMovementComponent->DefaultLandMovementMode)
			);

		UE_LOG(LogTemp, Warning, TEXT("%s"), *DebugMessage);

	}

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

	FString ActorName = GetName();
	int32 HashValue = FCrc::StrCrc32(*ActorName);
	FString MovementModeString = UEnum::GetValueAsString(CharacterMovementComponent->MovementMode);
	FString MoveInputIgnoredString = IsMoveInputIgnored() ? TEXT("True") : TEXT("False");

	FString DebugMessage = FString::Printf(TEXT("Actor: %s, Movement Mode: %s, Move Input Ignored: %s"),
		*ActorName,
		*MovementModeString,
		*MoveInputIgnoredString);

	GEngine->AddOnScreenDebugMessage(HashValue, 0.1f, FColor::Blue, DebugMessage);


	if (bOnBoard)
	{
		AddMovementInput(GetActorForwardVector(), 0.001f);
	}
}

void AXR_Character::InteractableEffectStart_Implementation()
{
	if (!GetCharacterMesh()) return;
	if (bHightLighting) return;

	bHightLighting = true;

	if (HighlightMaterial)
	{
		CharacterMesh->SetMaterial(0, HighlightMaterial);
		CharacterMesh->SetMaterial(1, HighlightMaterial);
	}
		

	FVector NewScale = CharacterMesh->GetRelativeScale3D() * rescaleAmount; 
	CharacterMesh->SetRelativeScale3D(NewScale);

}

void AXR_Character::InteractableEffectEnd_Implementation()
{
	if (!GetCharacterMesh()) return;
	if (!bHightLighting) return;

	bHightLighting = false;

	if (DefaultMaterialFirst) CharacterMesh->SetMaterial(0, DefaultMaterialFirst);
	if (DefaultMaterialSecond) CharacterMesh->SetMaterial(1, DefaultMaterialSecond);

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
	FromPaletteToCharacter->SetVisibility(true);
	FromCharacterToRing->SetVisibility(true);
	bPalletteBeamAvailable = true;

	PalletteBeamEndPosition = GetActorLocation();

	FromPaletteToCharacter->SetVectorParameter("User.BeamEnd", PalletteBeamEndPosition - FVector(0.0f, 0.0f, 3.f));
	XRGamePlayMode->TriggerOnObjectGrabEvent(true, ObjectType);

}

void AXR_Character::GrabEnd_Implementation()
{
	FromPaletteToCharacter->SetVisibility(false);
	FromCharacterToRing->SetVisibility(false);
	bPalletteBeamAvailable = false;

	XRGamePlayMode->TriggerOnObjectGrabEvent(false, ObjectType);

	if (bOnBoard) return;

	bOnBoard = FloorRingMesh->bBeneathBoard;

	if (bOnBoard)
	{
		OnSetBoardEvent.Broadcast(ObjectType, CharacterType, SpawnPlaceIndex);

		if (DissolveCurve && TimelineComponent)
		{
			InterpFunction.BindDynamic(this, &AXR_Character::DissolveCallBack);
			TimelineComponent->AddInterpFloat(DissolveCurve, InterpFunction, FName("Alpha"));
			TimelineComponent->SetLooping(false);
			TimelineComponent->SetIgnoreTimeDilation(true);
			TimelineComponent->SetTimelineLength(2.0f); 
			TimelineComponent->Play();
		}

	}
	else
	{
		SetInteractPosition_Implementation(PalletteBeamEndPosition);
	}


}

void AXR_Character::DissolveCallBack(float percent)
{
	GetMesh()->SetScalarParameterValueOnMaterials("Dissolve", percent);
	FloorRingMesh->ChangeRingColorRotation(percent, 10.f);
}

bool AXR_Character::IsOnBoard_Implementation()
{
	return bOnBoard;
}

