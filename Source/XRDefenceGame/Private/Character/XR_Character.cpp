// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/XR_Character.h"
#include "NiagaraComponent.h"
#include "Component/FloorRingSMC.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialInstance.h"
#include "XRDefenceGame/XRDefenceGame.h"
#include "Interface/BuffableInterface.h"
#include "UI/CharacterUI.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AI/XRAIController.h"
#include "Mode/XRGamePlayMode.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Animation/AnimMontage.h"



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
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	TurretTypeMap.Add(30 , ECharacterType::ECT_DefenceT_Laser_1);
	TurretTypeMap.Add(60 , ECharacterType::ECT_DefenceT_Laser_2);
	TurretTypeMap.Add(21 , ECharacterType::ECT_DefenceT_Canon_1);
	TurretTypeMap.Add(42 , ECharacterType::ECT_DefenceT_Canon_2);
	TurretTypeMap.Add(12 , ECharacterType::ECT_DefenceT_Fire_1);
	TurretTypeMap.Add(24 , ECharacterType::ECT_DefenceT_Fire_2);
	TurretTypeMap.Add(3 , ECharacterType::ECT_DefenceT_Arrow_1);
	TurretTypeMap.Add(6 , ECharacterType::ECT_DefenceT_Arrow_2);
}




void AXR_Character::OnBoardCalledFunction(bool isOnBoard, bool isSpawnedByHand)
{
	if (bOnBoard)
	{
		StartDissolveTimeline(true);

		GetWorld()->GetTimerManager().SetTimer(BehaviorAvailableTimerHandle, this, &AXR_Character::BehaviorAvailableTimerFunction, 2.0f, false);

		SpawnCharacterPropertyUI();
		FloorRingMesh->bCharacterOnBoard = true;

		if (isSpawnedByHand)
		{
			OnSetBoardEvent.Broadcast(ObjectType, CharacterType, SpawnPlaceIndex);
		}
	}

}

void AXR_Character::BeginPlay()
{
	Super::BeginPlay();

	InitializeCharacter();

	if (DefaultPlaceInBoard)
	{
		SetOnBoardAuto();
	}
}

void AXR_Character::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (BehaviorTree && NewController)
	{
		XRAIController = Cast<AXRAIController>(NewController);
		if (XRAIController)
		{
			
			XRAIController->GetBlackboardComponent()->InitializeBlackboard(*BehaviorTree->BlackboardAsset);
			XRAIController->RunBehaviorTree(BehaviorTree);
		}
	}
}


void AXR_Character::InitializeCharacter()
{
	if (!GetCharacterMesh()) return;

	DefaultSkeletalMaterialFirst = Cast<UMaterialInstance>(CharacterMesh->GetMaterial(0));
	DefaultSkeletalMaterialSecond = Cast<UMaterialInstance>(CharacterMesh->GetMaterial(1));

	XRGamePlayMode = Cast<AXRGamePlayMode>(UGameplayStatics::GetGameMode(this));
	if (XRGamePlayMode)
	{
		XRGamePlayMode->OnChrarcterDieEvent.AddDynamic(this, &AXR_Character::TargetDieCallBack);
	}

	SetRingProperty();
	CharacterMovementComponent->MaxWalkSpeed = 5.f;
	
}

void AXR_Character::NonPalletteSpawnInitalize(FCharacterValueTransmitForm inheritform)
{
	CharacterProperty.currentHealth = inheritform.currentHealth;

	SetOnBoardAuto();
}

void AXR_Character::SetOnBoardAuto()
{
	bOnBoard = true;
	OnBoardCalledFunction(true, false);

}

void AXR_Character::SpawnCharacterPropertyUI()
{
	if (characterProperyUIClass)
	{
		FVector SpawnLocation = GetActorLocation() + FVector(0, 0, 6);
		FRotator SpawnRotation = GetActorRotation();

		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = GetInstigator();

		CharacterPropertyUI = GetWorld()->SpawnActor<ACharacterUI>(characterProperyUIClass, SpawnLocation, SpawnRotation, SpawnParams);
		if (CharacterPropertyUI)
		{
			CharacterPropertyUI->AttachToComponent(GetCapsuleComponent(), FAttachmentTransformRules::KeepWorldTransform);

			SetPropertyUIVisible(false);
		}

		UpdateCharacterPropertyUI();

	}
}

void AXR_Character::UpdateCharacterPropertyUI()
{
	if (CharacterPropertyUI)
	{
		CharacterPropertyUI->SetHealthPercent(CharacterProperty.currentHealth / CharacterProperty.MaxHealth);
		CharacterPropertyUI->SetDamageCount(CharacterProperty.Damage);
		CharacterPropertyUI->SetUtilCount(CharacterProperty.Util_Fast);
	}
}


void AXR_Character::SetPropertyUIVisible(bool flag)
{
	if (!CharacterPropertyUI) return;


	if (ObjectType == EObjectType::EOT_Deffence)
	{
		CharacterPropertyUI->SetDamgeUtilVisible(true);
		return;
	}

	CharacterPropertyUI->SetDamgeUtilVisible(flag);
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

	FloorRingMesh->ownerCharacterType = CharacterType;

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
}

void AXR_Character::InteractableEffectStart_Implementation()
{
	if (!GetCharacterMesh()) return;
	if (bHightLighting) return;

	bHightLighting = true;

	SetPropertyUIVisible(true);

	HighLightMesh(true);
		
	FVector NewScale = CharacterMesh->GetRelativeScale3D() * rescaleAmount; 
	CharacterMesh->SetRelativeScale3D(NewScale);

}

void AXR_Character::HighLightMesh(bool bHighlight)
{
	if (bHighlight)
	{
		if (HighlightMaterial)
		{
			CharacterMesh->SetMaterial(0, HighlightMaterial);
			CharacterMesh->SetMaterial(1, HighlightMaterial);
		}
	}
	else
	{
		if (DefaultSkeletalMaterialFirst) CharacterMesh->SetMaterial(0, DefaultSkeletalMaterialFirst);
		if (DefaultSkeletalMaterialSecond) CharacterMesh->SetMaterial(1, DefaultSkeletalMaterialSecond);

	}

}

void AXR_Character::InteractableEffectEnd_Implementation()
{
	if (!GetCharacterMesh()) return;
	if (!bHightLighting) return;

	bHightLighting = false;

	SetPropertyUIVisible(false);

	HighLightMesh(false);

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

	SetPalletteCharacterOnBoard(FloorRingMesh->bBeneathBoard, FloorRingMesh->GetBuffableCharacter());

	if (!bOnBoard)
	{
		SetInteractPosition_Implementation(PalletteBeamEndPosition);
	}


}

void AXR_Character::SetPalletteCharacterOnBoard(bool isOnBoard, AXR_Character* beneathBuffableCharacter)
{
	bOnBoard = isOnBoard;

	if (isOnBoard)
	{
		if (beneathBuffableCharacter)
		{
			SetInteractPosition_Implementation(beneathBuffableCharacter->GetActorLocation());
			IBuffableInterface::Execute_BuffableEffectEnd(beneathBuffableCharacter);
		}

	}

	OnBoardCalledFunction(isOnBoard, true);

}

void AXR_Character::PackCharacterValueTransmitForm(FCharacterValueTransmitForm& outForm)
{
	outForm.currentHealth = CharacterProperty.currentHealth;
}

void AXR_Character::StartDissolveTimeline(bool bNotReverse)
{
	if (DissolveCurve && TimelineComponent)
	{
		TimelineComponent->Stop(); 
		TimelineComponent->SetNewTime(0.f); 

		if (bNotReverse)
		{
			TimelineComponent->SetPlayRate(1.0f);
			BindDissolveCallBack();
		}
		else
		{
			TimelineComponent->SetPlayRate(2.0f);
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
	FloorRingMesh->bTickReject = true;
	bOnBoard = false;

	StartDissolveTimeline(false);
	GetWorld()->GetTimerManager().SetTimer(DeathTimerHandle, this, &AXR_Character::DeathTimerFunction, 2.0f, false);
	
	XRGamePlayMode->OnChrarcterDieEvent.Broadcast(this);

	if (CharacterPropertyUI)
	{
		CharacterPropertyUI->Destroy();
		CharacterPropertyUI = nullptr;
	}

	SetAnimState(EAnimationState::EAS_Death);
}

void AXR_Character::DestroyMyself()
{
	Destroy();

	if (CharacterPropertyUI)
	{
		CharacterPropertyUI->Destroy();
		CharacterPropertyUI = nullptr;
	}
}


void AXR_Character::DeathTimerFunction()
{
	DestroyMyself();
}

void AXR_Character::BehaviorAvailableTimerFunction()
{
	bBehaviorAvailable = true;
}

void AXR_Character::DissolveCallBack(float percent)
{
	GetMesh()->SetScalarParameterValueOnMaterials("Dissolve", percent);
	FloorRingMesh->ChangeRingColorRotation(percent, 12.f);
}

void AXR_Character::DissolveCallBackReverse(float percent)
{
	percent = 1 - percent;
	GetMesh()->SetScalarParameterValueOnMaterials("Dissolve", percent);
	FloorRingMesh->ChangeRingColorRotation(percent, -12.f);

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

void AXR_Character::Heal(float healAmount)
{
	CharacterProperty.currentHealth += healAmount;
	CharacterProperty.currentHealth = FMath::Clamp(CharacterProperty.currentHealth, 0.f, CharacterProperty.MaxHealth);
	UpdateCharacterPropertyUI();
}

void AXR_Character::CharacterActionCall()
{

	if (AnimState <= EAnimationState::EAS_IdleAndWalk)
	{
		CharacterActionStart();
	}
}

void AXR_Character::CharacterActionImpact()
{

}

void AXR_Character::CharacterActionEnd()
{
	SetAnimState(EAnimationState::EAS_IdleAndWalk);
}

float AXR_Character::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (CharacterProperty.currentHealth == 0) return 0;

	CharacterProperty.currentHealth -= DamageAmount;
	CharacterProperty.currentHealth = FMath::Clamp(CharacterProperty.currentHealth, 0.f, CharacterProperty.MaxHealth);

	UpdateCharacterPropertyUI();

	if (CharacterProperty.currentHealth <= 0)
	{
		Death();
	}

	return DamageAmount;
}

void AXR_Character::TargetDieCallBack(AXR_Character* DieTarget)
{
	if (!TargetCharacter) return;
	if (!DieTarget) return;

	if (DieTarget == TargetCharacter)
	{
		
		/*
		FString ActorName = GetName();
		int32 HashValue = FCrc::StrCrc32(*ActorName);

		FString TargetCharacterName = TargetCharacter ? TargetCharacter->GetName() : TEXT("None");
		FString DieTargetName = DieTarget ? DieTarget->GetName() : TEXT("None");

		FString DebugMessage = FString::Printf(TEXT("Actor: %s, TargetCharacter: %s, DieTarget: %s"),
			*ActorName, *TargetCharacterName, *DieTargetName);

		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(HashValue, 5.0f, FColor::Blue, DebugMessage);
		}
		*/



		TargetCharacter = nullptr;
	}


}

void AXR_Character::SetAnimState(EAnimationState state)
{
	AnimState = state; 
}

void AXR_Character::CharacterActionStart()
{

	if (CharacterActionMontage && GetMesh()->GetAnimInstance())
	{
		SetAnimState(EAnimationState::EAS_Action);
		GetMesh()->GetAnimInstance()->Montage_Play(CharacterActionMontage);
	}
}

