// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/XR_Character.h"
#include "NiagaraComponent.h"
#include "Component/FloorRingSMC.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialInstance.h"
#include "XRDefenceGame/XRDefenceGame.h"
#include "Interface/BuffableInterface.h"

#include "GameFramework/CharacterMovementComponent.h"

#include "Mode/XRGamePlayMode.h"

AXR_Character::AXR_Character()
{
	PrimaryActorTick.bCanEverTick = true;

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

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
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

}


void AXR_Character::OnBoardCalledFunction(bool isOnBoard)
{

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

	DefaultSkeletalMaterialFirst = Cast<UMaterialInstance>(GetMesh()->GetMaterial(0));
	DefaultSkeletalMaterialSecond = Cast<UMaterialInstance>(GetMesh()->GetMaterial(1));

	XRGamePlayMode = Cast<AXRGamePlayMode>(UGameplayStatics::GetGameMode(this));

	SetRingProperty();

}



void AXR_Character::SetOnBoard(bool isOnBoard)
{
	bOnBoard = isOnBoard;

	OnBoardCalledFunction(isOnBoard);

	if (bOnBoard)
	{
		OnSetBoardEvent.Broadcast(ObjectType, CharacterType, SpawnPlaceIndex);
	}
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

	switch (CharacterType)
	{
	case ECharacterType::ECT_DefenceH :
	case ECharacterType::ECT_DefenceF1:
	case ECharacterType::ECT_DefenceF2:
		FloorRingMesh->SetMaterial(0, DefaultRingMaterial);
		FloorRingMesh->beneathTraceChannel = ECC_Buffable;
		break;
	}

	FloorRingMesh->SetMaterialCall();

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
	if (bHightLighting) return;

	bHightLighting = true;

	HighLightMesh(true);
		
	FVector NewScale = GetMesh()->GetRelativeScale3D() * rescaleAmount;
	GetMesh()->SetRelativeScale3D(NewScale);

}

void AXR_Character::HighLightMesh(bool bHighlight)
{
	if (bHighlight)
	{
		if (HighlightMaterial)
		{
			GetMesh()->SetMaterial(0, HighlightMaterial);
			GetMesh()->SetMaterial(1, HighlightMaterial);
		}
	}
	else
	{
		if (DefaultSkeletalMaterialFirst) GetMesh()->SetMaterial(0, DefaultSkeletalMaterialFirst);
		if (DefaultSkeletalMaterialSecond) GetMesh()->SetMaterial(1, DefaultSkeletalMaterialSecond);

	}

}



void AXR_Character::InteractableEffectEnd_Implementation()
{
	if (!bHightLighting) return;

	bHightLighting = false;

	HighLightMesh(false);

	FVector NewScale = GetMesh()->GetRelativeScale3D() / rescaleAmount;
	GetMesh()->SetRelativeScale3D(NewScale);


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

	SetOnBoard(FloorRingMesh->bBeneathBoard);


	if (bOnBoard)
	{
		AXR_Character* beneathBuffableCharacter = FloorRingMesh->GetBuffableCharacter();

		if (beneathBuffableCharacter)
		{
			IBuffableInterface* beneathBuffable = Cast<IBuffableInterface>(beneathBuffableCharacter);
			SetInteractPosition_Implementation(beneathBuffableCharacter->GetActorLocation());
			IBuffableInterface::Execute_BuffableEffectEnd(beneathBuffableCharacter);
		}

		StartDissolveTimeline(true);

	}
	else
	{
		SetInteractPosition_Implementation(PalletteBeamEndPosition);
	}


}

void AXR_Character::StartDissolveTimeline(bool bNotReverse)
{
	if (DissolveCurve && TimelineComponent)
	{
		if (bNotReverse)
		{
			BindDissolveCallBack();
		}
		else
		{
			BindReverseDissolveCallBack();
		}

		TimelineComponent->AddInterpFloat(DissolveCurve, InterpFunction, FName("Alpha"));
		TimelineComponent->SetLooping(false);
		TimelineComponent->SetIgnoreTimeDilation(true);
		TimelineComponent->SetTimelineLength(2.0f);
		TimelineComponent->Play();
	}
}

void AXR_Character::Death()
{
	bOnBoard = false;
	GetWorld()->GetTimerManager().SetTimer(DeathTimerHandle, this, &AXR_Character::DeathTimerFunction, 2.0f, false);
	StartDissolveTimeline(false);
}

void AXR_Character::DeathTimerFunction()
{
	Destroy();
}

void AXR_Character::DissolveCallBack(float percent)
{
	GetMesh()->SetScalarParameterValueOnMaterials("Dissolve", percent);
	FloorRingMesh->ChangeRingColorRotation(percent, 12.f);
}

void AXR_Character::DissolveCallBackReverse(float percent)
{
	DissolveCallBack(1- percent);
}

void AXR_Character::BindDissolveCallBack()
{
	InterpFunction.BindDynamic(this, &AXR_Character::DissolveCallBack);
}

void AXR_Character::BindReverseDissolveCallBack()
{
	InterpFunction.BindDynamic(this, &AXR_Character::DissolveCallBackReverse);
}


bool AXR_Character::IsOnBoard_Implementation()
{
	return bOnBoard;
}

