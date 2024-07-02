// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/XR_Character.h"
#include "NiagaraComponent.h"
#include "Component/FloorRingSMC.h"
#include "Components/CapsuleComponent.h"
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

	bPalletteBeamAvailable = false;
	FromPaletteToCharacter->SetVisibility(false);
	FromCharacterToRing->SetVisibility(false);

	CharacterMovementComponent = GetCharacterMovement();
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

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

	DefaultSkeletalMaterialFirst = Cast<UMaterialInstance>(CharacterMesh->GetMaterial(0));
	DefaultSkeletalMaterialSecond = Cast<UMaterialInstance>(CharacterMesh->GetMaterial(1));

	XRGamePlayMode = Cast<AXRGamePlayMode>(UGameplayStatics::GetGameMode(this));

	SetRingProperty();

}

void AXR_Character::CheckNeutralToConvert(EObjectType objectType)
{
	if (EObjectType::EOT_Neutral != ObjectType) return;

	if (CharacterType == ECharacterType::ECT_Gold)
	{
		if (objectType == EObjectType::EOT_Offence)
		{
			ObjectType = EObjectType::EOT_OffenceGold;
		}
		else if (objectType == EObjectType::EOT_Deffence)
		{
			ObjectType = EObjectType::EOT_DeffenceGold;
		}
	}
	else
	{
		ObjectType = objectType;
	}

	SetRingProperty();
}

void AXR_Character::SetRingProperty()
{
	switch (ObjectType)
	{
	case EObjectType::EOT_Offence:
		FloorRingMesh->SetMaterial(0, OffenceRingMaterial);
		FloorRingMesh->beneathTraceChannel = ECC_AttackBoard;
		break;

	case EObjectType::EOT_OffenceGold:
		FloorRingMesh->SetMaterial(0, OffenceRingMaterial);
		FloorRingMesh->beneathTraceChannel = ECC_AttackGoldBoard;
		break;

	case EObjectType::EOT_Deffence:
		FloorRingMesh->SetMaterial(0, DefenceRingMaterial);
		FloorRingMesh->beneathTraceChannel = ECC_DefenceBoard;
		break;

	case EObjectType::EOT_DeffenceGold:
		FloorRingMesh->SetMaterial(0, DefenceRingMaterial);
		FloorRingMesh->beneathTraceChannel = ECC_DefenceGoldBoard;
		break;

	case EObjectType::EOT_Neutral:
		FloorRingMesh->SetMaterial(0, DefaultRingMaterial);
		FloorRingMesh->beneathTraceChannel = ECC_Board;
		break;
	case EObjectType::EOT_None:
		break;
	default:
		break;
	}

	FloorRingMesh->SetMaterialCall();

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

	/*
	
	FString ActorName = GetName();
	int32 HashValue = FCrc::StrCrc32(*ActorName);
	FString MovementModeString = UEnum::GetValueAsString(CharacterMovementComponent->MovementMode);
	FString MoveInputIgnoredString = IsMoveInputIgnored() ? TEXT("True") : TEXT("False");

	FString DebugMessage = FString::Printf(TEXT("Actor: %s, Movement Mode: %s, Move Input Ignored: %s"),
		*ActorName,
		*MovementModeString,
		*MoveInputIgnoredString);

	GEngine->AddOnScreenDebugMessage(HashValue, 0.1f, FColor::Blue, DebugMessage);
	
	*/

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

	if (DefaultSkeletalMaterialFirst) CharacterMesh->SetMaterial(0, DefaultSkeletalMaterialFirst);
	if (DefaultSkeletalMaterialSecond) CharacterMesh->SetMaterial(1, DefaultSkeletalMaterialSecond);

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
			BindDissolveCallBack();
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
	FloorRingMesh->ChangeRingColorRotation(percent, 12.f);
}

void AXR_Character::BindDissolveCallBack()
{
	InterpFunction.BindDynamic(this, &AXR_Character::DissolveCallBack);
}

bool AXR_Character::IsOnBoard_Implementation()
{
	return bOnBoard;
}

