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
#include "Components/SphereComponent.h"
#include "DrawDebugHelpers.h"
#include "MotionWarpingComponent.h"
#include "Managet/AudioSubsystem.h"
#include "Managet/XRDefenceGameInstance.h"
#include "Battle/CostShowChip.h"
#include "Net/UnrealNetwork.h"
#include "NiagaraSystemInstanceController.h"

AXR_Character::AXR_Character()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	FromPaletteToCharacter = CreateDefaultSubobject<UNiagaraComponent>(FName("FromPaletteToCharacter"));
	FromPaletteToCharacter->SetupAttachment(RootComponent);
	FromPaletteToCharacter->SetIsReplicated(true);

	FromCharacterToRing = CreateDefaultSubobject<UNiagaraComponent>(FName("FromCharacterToRing"));
	FromCharacterToRing->SetupAttachment(RootComponent);
	FromCharacterToRing->SetIsReplicated(true);

	BuffRing = CreateDefaultSubobject<UNiagaraComponent>(FName("BuffRing"));
	BuffRing->SetupAttachment(GetMesh());
	BuffRing->SetIsReplicated(true);

	HealRing = CreateDefaultSubobject<UNiagaraComponent>(FName("HealRing"));
	HealRing->SetupAttachment(GetMesh());
	HealRing->SetIsReplicated(true);

	SpeedBuffNiagara = CreateDefaultSubobject<UNiagaraComponent>(FName("SpeedBuffNiagara"));
	SpeedBuffNiagara->SetupAttachment(GetMesh());
	SpeedBuffNiagara->SetIsReplicated(true);

	
	MotionWarpingComponent = CreateDefaultSubobject<UMotionWarpingComponent>(FName("MotionWarping"));

	FloorRingMesh = CreateDefaultSubobject<UFloorRingSMC>(FName("FloorRingMesh"));
	FloorRingMesh->SetupAttachment(RootComponent);
	FloorRingMesh->SetIsReplicated(true);

	TimelineComponent = CreateDefaultSubobject<UTimelineComponent>(TEXT("DissolveTimeline"));

	bPalletteBeamAvailable = false;
	FromPaletteToCharacter->SetVisibility(false);
	FromCharacterToRing->SetVisibility(false);

	CharacterMovementComponent = GetCharacterMovement();
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	GetMesh()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Bullet, ECollisionResponse::ECR_Block);

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

	GetCapsuleComponent()->SetWorldScale3D(FVector(0.045f, 0.045f, 0.045f));

	sphereOverlapCheck = CreateDefaultSubobject<USphereComponent>(FName("Sphere Overlap"));
	sphereOverlapCheck->SetSphereRadius(0.01f);
	sphereOverlapCheck->SetupAttachment(GetCapsuleComponent());
	sphereOverlapCheck->SetCollisionResponseToAllChannels(ECR_Ignore);
	sphereOverlapCheck->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

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
	if (HasAuthority())
	{
		OnBoardCalledFunctionServer(isOnBoard, isSpawnedByHand);
	}

}

void AXR_Character::OnBoardCalledFunctionServer(bool isOnBoard, bool isSpawnedByHand)
{
	if (bOnBoard)
	{
		StartDissolveTimeline(true);

		GetWorld()->GetTimerManager().SetTimer(BehaviorAvailableTimerHandle, this, &AXR_Character::BehaviorAvailableTimerFunction, 2.0f, false);

		PlaySoundViaManager(EGameSoundType::EGST_SFX, SoundSpawnBoard, GetActorLocation(), 0.7f);

		SpawnCharacterPropertyUI();

		FloorRingMesh->SetbCharacterOnBoard(true);


		if (XRGamePlayMode)
		{
			XRGamePlayMode->OnCharacterSpawnEvent.Broadcast(GetActorLocation());
			if (isSpawnedByHand) XRGamePlayMode->OnCostEvent.Broadcast(ObjectType ,CharacterProperty.Cost);
		}

		if (isSpawnedByHand)
		{
			CallBackForPallette();
		}
	}

}

void AXR_Character::BeginPlay()
{
	Super::BeginPlay();

	GameInstance = Cast<UXRDefenceGameInstance>(GetWorld()->GetGameInstance());

	XRGamePlayMode = Cast<AXRGamePlayMode>(UGameplayStatics::GetGameMode(this));

	if (!GetCharacterMesh()) return;

	DefaultSkeletalMaterialFirst = Cast<UMaterialInstance>(CharacterMesh->GetMaterial(0));
	DefaultSkeletalMaterialSecond = Cast<UMaterialInstance>(CharacterMesh->GetMaterial(1));

	SetRingProperty();

	HealRing->Deactivate();
	BuffRing->Deactivate();
	SpeedBuffNiagara->Deactivate();

	if (HealRing)
	{
		FNiagaraSystemInstanceControllerPtr SystemInstanceController = HealRing->GetSystemInstanceController();
		if (SystemInstanceController)
		{
			SystemInstanceController->SetForceSolo(true);
		}
	}

	if (BuffRing)
	{
		FNiagaraSystemInstanceControllerPtr SystemInstanceController = BuffRing->GetSystemInstanceController();
		if (SystemInstanceController)
		{
			SystemInstanceController->SetForceSolo(true);
		}
	}

	if (SpeedBuffNiagara)
	{
		FNiagaraSystemInstanceControllerPtr SystemInstanceController = SpeedBuffNiagara->GetSystemInstanceController();
		if (SystemInstanceController)
		{
			SystemInstanceController->SetForceSolo(true);
		}
	}


	if (HasAuthority())
	{
		ActorNetID = GetUniqueID();
		if (XRGamePlayMode)
		{
			XRGamePlayMode->AddActorToMap(ActorNetID, this);
		}
	}

	if (HasAuthority())
	{
		InitializeCharacter();

		if (DefaultPlaceInBoard)
		{
			SetOnBoardAuto();
		}
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
	if (XRGamePlayMode)
	{
		XRGamePlayMode->OnChrarcterDieEvent.AddDynamic(this, &AXR_Character::TargetDieCallBack);
		XRGamePlayMode->OnCharacterSpawnEvent.AddDynamic(this, &AXR_Character::OtherCharacterSpawnCallBack);
		XRGamePlayMode->OnGameEnd.AddDynamic(this, &AXR_Character::GameEndCallBack);
	}

	FHitResult PallettetraceResult;
	GetWorld()->LineTraceSingleByChannel(PallettetraceResult, GetActorLocation(), GetActorLocation() + FVector::DownVector * 100.f, ECC_Pallette);
	
	if (PallettetraceResult.bBlockingHit)
	{
		SpawnCostShowUI();
	}

	CharacterMovementComponent->MaxWalkSpeed = 5.f;

	if (ObjectType == EObjectType::EOT_Deffence) 
	{
		sphereOverlapCheck->SetWorldScale3D(FVector(1.0f));
		sphereOverlapCheck->SetSphereRadius(CharacterProperty.Util_Range);

		sphereOverlapCheck->OnComponentBeginOverlap.AddDynamic(this, &AXR_Character::OnSphereOverlapBegin);
	}


	
}

void AXR_Character::GameEndCallBack()
{
	bBehaviorAvailable = false;
}


void AXR_Character::NonPalletteSpawnInitalize(FCharacterValueTransmitForm inheritform)
{
	CharacterProperty.currentHealth = inheritform.currentHealth + (CharacterProperty.MaxHealth -inheritform.beforeMaxHealth);
	SetOnBoardAuto();
}

void AXR_Character::SetOnBoardAuto()
{
	bOnBoard = true;
	OnBoardCalledFunction(true, false);	

}

void AXR_Character::SpawnCostShowUI()
{
	if (CostShowUI != nullptr) return;

	if (costShowUIClass)
	{
		FVector SpawnLocation = GetActorLocation() - GetActorForwardVector() * 4;
		FRotator SpawnRotation = GetActorRotation();

		if (GetMesh())
		{
			FHitResult PallettetraceResult;
			GetWorld()->LineTraceSingleByChannel(PallettetraceResult, GetActorLocation(), GetActorLocation() + FVector::DownVector * 100.f, ECC_Pallette);

			if (PallettetraceResult.bBlockingHit)
			{
				SpawnLocation.Z = PallettetraceResult.ImpactPoint.Z + 0.5f;
			}
			else
			{
				SpawnLocation.Z = GetMesh()->GetComponentLocation().Z + 0.25f;
			}
		}

		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = GetInstigator();

		CostShowUI = GetWorld()->SpawnActor<ACostShowChip>(costShowUIClass, SpawnLocation, SpawnRotation, SpawnParams);
		if (CostShowUI)
		{
			CostShowUI->SetGoldCostCountMulti(CharacterProperty.Cost);
		}
	}
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
		CharacterPropertyUI->SetHealthPercentMulticast(CharacterProperty.currentHealth / CharacterProperty.MaxHealth);
		CharacterPropertyUI->SetDamageCountMulticast(CharacterProperty.currentDamage);
		CharacterPropertyUI->SetUtilCountMulticast(CharacterProperty.Util_Fast);
	}

}

void AXR_Character::SetPropertyUIVisible(bool flag)
{
	if (!CharacterPropertyUI) return;

	if (CharacterType == ECharacterType::ECT_DefenceNexus)
	{
		CharacterPropertyUI->SetDamgeUtilVisibleMulticast(false);
		return;
	}


	if (ObjectType == EObjectType::EOT_Deffence && CharacterType != ECharacterType::ECT_DefenceP)
	{
		CharacterPropertyUI->SetDamgeUtilVisibleMulticast(true);
		return;
	}

	if (CharacterType == ECharacterType::ECT_DefenceP || CharacterType == ECharacterType::ECT_OffenceS)
	{
		flag = false;
	}

	CharacterPropertyUI->SetDamgeUtilVisibleMulticast(flag);
}


void AXR_Character::CheckNeutralToConvert(EObjectType objectType)
{
	if (EObjectType::EOT_Neutral != ObjectType) return;

	if (CharacterType == ECharacterType::ECT_Gold)
	{
		if (XRGamePlayMode)
		{
			XRGamePlayMode->AddGoldCount(objectType);
		}

		if (objectType == EObjectType::EOT_Offence)
		{
			ObjectType = EObjectType::EOT_OffenceGold;

			if (XRGamePlayMode)
			{
				CharacterProperty.Cost = XRGamePlayMode->OffenceGoldCount * 20 - 10;
			}
		}
		else if (objectType == EObjectType::EOT_Deffence)
		{
			ObjectType = EObjectType::EOT_DeffenceGold;

			if (XRGamePlayMode)
			{
				CharacterProperty.Cost = XRGamePlayMode->DefenceGoldCount * 20 - 10;
			}
		}

		if (CostShowUI == nullptr)
		{
			SpawnCostShowUI();
		}
		else
		{
			CostShowUI->SetGoldCostCountMulti(CharacterProperty.Cost);
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



FVector AXR_Character::GetRingPosition()
{
	 return FloorRingMesh->GetComponentLocation(); 
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

	if (!bOnBoard)
	{
		FromCharacterToRing->SetVectorParameter("User.BeamEnd", FloorRingMesh->GetComponentLocation());
	}
}

void AXR_Character::InteractableEffectStart_Implementation()
{
	if (!GetCharacterMesh()) return;
	if (bHightLighting) return;

	if (HasAuthority())
	{
		Multi_InteractableEffectStart();
	}
	else
	{
		Server_InteractableEffectStart();
	}
}

void AXR_Character::Server_InteractableEffectStart_Implementation()
{

	Multi_InteractableEffectStart();
}

void AXR_Character::Multi_InteractableEffectStart_Implementation()
{

	PlaySoundViaManager(EGameSoundType::EGST_SFX, SoundHighLight, GetActorLocation(), 1.0f, true);

	bHightLighting = true;

	SetPropertyUIVisible(true);

	ChangeMaterialState(EMaterialState::EMS_HandHighLight, true);

	FVector NewScale = CharacterMesh->GetRelativeScale3D() * rescaleAmount;
	CharacterMesh->SetRelativeScale3D(NewScale);
}


void AXR_Character::InteractableEffectEnd_Implementation()
{
	if (!GetCharacterMesh()) return;
	if (!bHightLighting) return;

	if (HasAuthority())
	{
		Multi_InteractableEffectEnd();
	}
	else
	{
		Server_InteractableEffectEnd();
	}

}

void AXR_Character::Server_InteractableEffectEnd_Implementation()
{
	Multi_InteractableEffectEnd();
}

void AXR_Character::Multi_InteractableEffectEnd_Implementation()
{
	bHightLighting = false;

	SetPropertyUIVisible(false);

	ChangeMaterialState(EMaterialState::EMS_HandHighLight, false);

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
	if (HasAuthority())
	{
		Multi_SetInteractPosition(GrabPosition);
	}
	else
	{
		Server_SetInteractPosition(GrabPosition);
	}

}

void AXR_Character::Server_SetInteractPosition_Implementation(FVector GrabPosition)
{
	Multi_SetInteractPosition(GrabPosition);
}

void AXR_Character::Multi_SetInteractPosition_Implementation(FVector GrabPosition)
{
	SetActorLocation(GrabPosition);
}

void AXR_Character::GrabStart_Implementation ()
{
	if (HasAuthority())
	{
		SetReplicateMovement(false);
		Multi_GrabStart();
	}
	else
	{
		Server_GrabStart();
	}
}

void AXR_Character::Server_GrabStart_Implementation()
{
	Multi_GrabStart();
}

void AXR_Character::Multi_GrabStart_Implementation()
{
	if (XRGamePlayMode)
	{
		XRGamePlayMode->TriggerOnObjectGrabEvent(true, ObjectType);

	}



	FromPaletteToCharacter->SetVisibility(true);
	FromCharacterToRing->SetVisibility(true);
	bPalletteBeamAvailable = true;

	PalletteBeamEndPosition = GetActorLocation();

	FromPaletteToCharacter->SetVectorParameter("User.BeamEnd", PalletteBeamEndPosition - FVector(0.0f, 0.0f, 3.f));


}

void AXR_Character::GrabEnd_Implementation()
{
	if (HasAuthority())
	{

		SetReplicateMovement(true);
		Multi_GrabEnd();
	}
	else
	{
		Server_GrabEnd();
	}
}

EObjectType AXR_Character::GetInteractObjectType_Implementation()
{
	return GetObjectType();
}

void AXR_Character::Server_GrabEnd_Implementation()
{
	Multi_GrabEnd();
}

void AXR_Character::Multi_GrabEnd_Implementation()
{

	if (XRGamePlayMode)
	{
		XRGamePlayMode->TriggerOnObjectGrabEvent(false, ObjectType);

	}

	FromPaletteToCharacter->SetVisibility(false);
	FromCharacterToRing->SetVisibility(false);
	bPalletteBeamAvailable = false;

	if (bOnBoard) return;

	if (FloorRingMesh->bBeneathTrash)
	{
		Death(true);
		CallBackForPallette();
		PlaySoundViaManager(EGameSoundType::EGST_SFX, SoundDeathInTrash, GetActorLocation(), 0.5f);

		return;
	}
	else
	{
		SetPalletteCharacterOnBoard(FloorRingMesh->bBeneathBoard, FloorRingMesh->GetBuffableCharacter());
	}

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
	outForm.beforeMaxHealth = CharacterProperty.MaxHealth;
}

void AXR_Character::UpdateMotoionWarpingTransform()
{
	if (MotionWarpingComponent && TargetCharacter)
	{
		MotionWarpingComponent->Activate(true);
		MotionWarpingComponent->AddOrUpdateWarpTargetFromTransform(FName("CombatTarget"), TargetCharacter->GetActorTransform());
	}
}

void AXR_Character::TriggerHealEffect_Implementation()
{
	HealRing->Deactivate();
	HealRing->Activate(true);
}

void AXR_Character::TriggerBuffEffect_Implementation()
{
	BuffRing->Deactivate();
	BuffRing->Activate(true);
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
			InterpFunction.BindDynamic(this, &AXR_Character::DissolveCallBackMulti);
		}
		else
		{
			TimelineComponent->SetPlayRate(2.0f);
			InterpFunction.BindDynamic(this, &AXR_Character::DissolveCallBackReverseMulti);
		}

		TimelineComponent->AddInterpFloat(DissolveCurve, InterpFunction, FName("Alpha"));
		TimelineComponent->SetLooping(false);
		TimelineComponent->SetIgnoreTimeDilation(true);
		TimelineComponent->SetTimelineLength(2.0f);
		TimelineComponent->Play();
	}
}

void AXR_Character::Death(bool bDieInTrash)
{
	if (HasAuthority())
	{
		bOnBoard = false;
		bBehaviorAvailable = false;
		StartDissolveTimeline(false);

		GetWorld()->GetTimerManager().SetTimer(DeathTimerHandle, this, &AXR_Character::DeathTimerFunction, 2.0f, false);
		
		SetAnimState(EAnimationState::EAS_Death);

		if (!bDieInTrash)
		{
			PlaySoundViaManager(EGameSoundType::EGST_SFX, SoundDeath, GetActorLocation(), 0.5f);

			if (CharacterPropertyUI)
			{
				CharacterPropertyUI->Destroy();
				CharacterPropertyUI = nullptr;
			}
		}

		CharacterMovementComponent->MaxWalkSpeed = 0.f;
		FloorRingMesh->SetbTickReject(true);


		ActorNetID = GetUniqueID();
		if (XRGamePlayMode)
		{
			XRGamePlayMode->RemoveActorFromMap(ActorNetID);

			if (bDieInTrash)
			{
				XRGamePlayMode->OnCostEvent.Broadcast(ObjectType, 2);
			}
			else
			{
				XRGamePlayMode->OnChrarcterDieEvent.Broadcast(this);
			}
		}

		

		PlayAnimMontageMulti(GetMesh(), CharacterDeathMontage);
	}

}


void AXR_Character::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AXR_Character, AnimState);
	DOREPLIFETIME(AXR_Character, CharacterProperty);
	DOREPLIFETIME(AXR_Character, bBehaviorAvailable);
	DOREPLIFETIME(AXR_Character, bOnBoard);
	DOREPLIFETIME(AXR_Character, TargetCharacter);
	DOREPLIFETIME(AXR_Character, TargetCharacter2);
	DOREPLIFETIME(AXR_Character, bNowStun);
	DOREPLIFETIME(AXR_Character, bDisableInteractable);
	DOREPLIFETIME(AXR_Character, ActorNetID);
	DOREPLIFETIME(AXR_Character, ObjectType);


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

void AXR_Character::DamageTimerFunction()
{
	ChangeMaterialState(EMaterialState::EMS_Damage, false);
}

void AXR_Character::SetTrashEffect(bool flag, bool onlyNiagara)
{
	GetCharacterMesh();

	if (flag)
	{
		FromCharacterToRing->SetVariableLinearColor(FName("LineColor"), FLinearColor::Red);

		if (DisableHighlightMaterial && CharacterMesh->GetMaterial(0) == HighlightMaterial)
		{
			if (!onlyNiagara)
			{
				CharacterMesh->SetMaterial(0, DisableHighlightMaterial);
				CharacterMesh->SetMaterial(1, DisableHighlightMaterial);
			}

		}
	}
	else
	{
		FromCharacterToRing->SetVariableLinearColor(FName("LineColor"), FLinearColor::Blue);

		if (HighlightMaterial && CharacterMesh->GetMaterial(0) == DisableHighlightMaterial)
		{

			if (!onlyNiagara)
			{
				CharacterMesh->SetMaterial(0, HighlightMaterial);
				CharacterMesh->SetMaterial(1, HighlightMaterial);

			}

		}
	}
}

void AXR_Character::TriggerStun()
{
	bNowStun = true;
	ChangeMaterialState(EMaterialState::EMS_Stun, true);
	GetWorld()->GetTimerManager().SetTimer(StunTimerHandle, this, &AXR_Character::StunEndTimerFunction, StunTime, false);
}

void AXR_Character::StunEndTimerFunction()
{
	ChangeMaterialState(EMaterialState::EMS_Stun, false);
	bNowStun = false;
}

void AXR_Character::CallBackForPallette()
{
	if (CostShowUI)
	{
		CostShowUI->Destroy();
	}

	OnSetBoardEvent.Broadcast(ObjectType, CharacterType, SpawnPlaceIndex);

}

void AXR_Character::OnSphereOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && (OtherActor != this) && OtherComp && Cast<AXR_Character>(OtherActor))
	{

	}
}

void AXR_Character::DissolveCallBack(float percent)
{
	GetMesh()->SetScalarParameterValueOnMaterials("Dissolve", percent);
	GetMesh()->SetScalarParameterValueOnMaterials("Dark", 1-percent);

	FloorRingMesh->ChangeRingColorRotation(percent, 12.f);
}

void AXR_Character::DissolveCallBackReverse(float percent)
{
	percent = 1 - percent;
	GetMesh()->SetScalarParameterValueOnMaterials("Dissolve", percent);
	GetMesh()->SetScalarParameterValueOnMaterials("Dark", 1 - percent);
	FloorRingMesh->ChangeRingColorRotation(percent, -12.f);

}

void AXR_Character::DissolveCallBackMulti_Implementation(float percent)
{
	DissolveCallBack(percent);
}

void AXR_Character::DissolveCallBackReverseMulti_Implementation(float percent)
{
	DissolveCallBackReverse(percent);
}



bool AXR_Character::IsOnBoard_Implementation()
{
	return bOnBoard;
}

float AXR_Character::GetCost_Implementation()
{
	return CharacterProperty.Cost;
}

int32 AXR_Character::GetNetId_Implementation()
{
	return ActorNetID;
}

void AXR_Character::SetDisableHighLight_Implementation(bool bDisable)
{
	if (HasAuthority())
	{
		Multi_SetbDisableInteractable(bDisable);
	}
	else
	{
		Server_SetbDisableInteractable(bDisable);
	}

}

void AXR_Character::Server_SetbDisableInteractable_Implementation(bool flag)
{
	Multi_SetbDisableInteractable(flag);
}

void AXR_Character::Multi_SetbDisableInteractable_Implementation(bool flag)
{
	if (bOnBoard) return;
	if (bDisableInteractable == flag) return;

	bDisableInteractable = flag;

	SetTrashEffect(bDisableInteractable);
}

bool AXR_Character::GetDisableHighLight_Implementation()
{
	return bDisableInteractable;
}

void AXR_Character::Heal(float healAmount)
{
	CharacterProperty.currentHealth += healAmount;
	CharacterProperty.currentHealth = FMath::Clamp(CharacterProperty.currentHealth, 0.f, CharacterProperty.MaxHealth);
	TriggerHealEffect();

	UpdateCharacterPropertyUI();
}

void AXR_Character::AttackBuff(float BuffAmount)
{
	CharacterProperty.currentDamage = CharacterProperty.defaultDamage + BuffAmount;
	GetWorld()->GetTimerManager().SetTimer(BuffTimerHandle, this, &AXR_Character::BuffEndTimerFunction, BuffTime, false);
	TriggerBuffEffect();
}

void AXR_Character::BuffEndTimerFunction()
{
	CharacterProperty.currentDamage = CharacterProperty.defaultDamage;
}

void AXR_Character::CharacterActionCall()
{
	if (!HasAuthority()) return;

	if (GetIsNowCanWalkBasedOnAnimation())
	{
		CharacterActionStart();
	}
}

void AXR_Character::CharacterActionImpact()
{

}

void AXR_Character::CharacterActionSound()
{
	PlaySoundViaManager(EGameSoundType::EGST_SFX, SoundAction, GetActorLocation(), 0.6f);

}

void AXR_Character::CharacterActionImpact2()
{
}

void AXR_Character::CharacterActionEnd()
{
	SetAnimState(EAnimationState::EAS_IdleAndWalk);

}

void AXR_Character::FindNearbyEnemy(AXR_Character*& outFirstNear, AXR_Character*& outSecondNear)
{
	outFirstNear = nullptr;
	outSecondNear = nullptr;
}

float AXR_Character::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (HasAuthority())
	{
		if (CharacterProperty.currentHealth == 0) return DamageAmount;

		CharacterProperty.currentHealth -= DamageAmount;
		CharacterProperty.currentHealth = FMath::Clamp(CharacterProperty.currentHealth, 0.f, CharacterProperty.MaxHealth);

		UpdateCharacterPropertyUI();

		GetWorld()->GetTimerManager().SetTimer(DamageTimerHandle, this, &AXR_Character::DamageTimerFunction, 0.15f, false);

		ChangeMaterialState(EMaterialState::EMS_Damage, true);
		PlaySoundViaManager(EGameSoundType::EGST_SFX, SoundDamaged, GetActorLocation(), 0.1f);


		if (CharacterProperty.currentHealth <= 0)
		{
			Death(false);
		}
	}

	return DamageAmount;

}


void AXR_Character::TargetDieCallBack(AXR_Character* DieTarget)
{
	if (!DieTarget) return;

	if (TargetCharacter && DieTarget == TargetCharacter)
	{
		TargetCharacter = nullptr;

		if (TargetCharacter2)
		{
			TargetCharacter = TargetCharacter2;
			TargetCharacter2 = nullptr;
		}
	}

	if (TargetCharacter2 && DieTarget == TargetCharacter2)
	{
		TargetCharacter2 = nullptr;
	}


}

void AXR_Character::OtherCharacterSpawnCallBack(FVector spawnLocation)
{

}


void AXR_Character::SetAnimState(EAnimationState state)
{
	AnimState = state; 

}

void AXR_Character::TriggerMoveSlow()
{
	MoveSpeedDown(true);

	GetWorld()->GetTimerManager().SetTimer(MoveSpeedDownHandle, [this]()
		{
			this->MoveSpeedUp(false);
		}, 2.0f, false);
}

void AXR_Character::TriggerMoveFast()
{
	MoveSpeedUp(true);
	GetWorld()->GetTimerManager().SetTimer(MoveSpeedDownHandle, [this]()
		{
			this->MoveSpeedDown(false);
		}
		, 2.0f, false);
}

void AXR_Character::MoveSpeedUp_Implementation(bool bEffectOn)
{
	if (bEffectOn)
	{
		SpeedBuffNiagara->Deactivate();
		SpeedBuffNiagara->SetVariableLinearColor(FName("BodyColor"), FLinearColor::Blue);
		SpeedBuffNiagara->Activate(true);
	}

	CharacterMovementComponent->MaxWalkSpeed = CharacterMovementComponent->MaxWalkSpeed + 2.f;
}

void AXR_Character::MoveSpeedDown_Implementation(bool bEffectOn)
{
	if (bEffectOn)
	{
		SpeedBuffNiagara->Deactivate();
		SpeedBuffNiagara->SetVariableLinearColor(FName("BodyColor"), FLinearColor::Red);
		SpeedBuffNiagara->Activate(true);
	}

	CharacterMovementComponent->MaxWalkSpeed = CharacterMovementComponent->MaxWalkSpeed - 2.f;
}


void AXR_Character::PlaySoundViaManager(EGameSoundType soundType, USoundBase* Sound, FVector Location, float VolumeScale, bool bLocal)
{
	if (bLocal)
	{
		PlaySound(soundType, Sound, Location, VolumeScale);
	}
	else
	{
		if (HasAuthority())
		{
			Multi_PlaySound(soundType, Sound, Location, VolumeScale);
		}
		else
		{
			Server_PlaySound(soundType, Sound, Location, VolumeScale);
		}
	}


}

void AXR_Character::Multi_PlaySound_Implementation(EGameSoundType soundType, USoundBase* Sound, FVector Location, float VolumeScale)
{
	PlaySound(soundType, Sound, Location, VolumeScale);
}

void AXR_Character::Server_PlaySound_Implementation(EGameSoundType soundType, USoundBase* Sound, FVector Location, float VolumeScale)
{
	Multi_PlaySound(soundType, Sound, Location, VolumeScale);
}

void AXR_Character::PlaySound(EGameSoundType soundType, USoundBase* Sound, FVector Location, float VolumeScale)
{
	if (GameInstance)
	{
		AudioManager = (AudioManager == nullptr) ? GameInstance->GetAudioManagerSubsystem() : AudioManager;
		if (AudioManager)
		{
			AudioManager->PlaySound(soundType, Sound, Location, (SoundHighLight == Sound) ? VolumeScale : (VolumeScale * OwnVolumeScale));
		}
	}
}

void AXR_Character::ChangeMaterialState_Implementation(EMaterialState materialState, bool bLock)
{
	if (!GetCharacterMesh()) return;

	switch (materialState)
	{
	case EMaterialState::EMS_OnBoardHighLight:
		bLockOnBoardHighLight = bLock;
		break;

	case EMaterialState::EMS_Stun:
		bLockOnStun = bLock;
		break;

	case EMaterialState::EMS_Damage:
		bLockDamage = bLock;
		break;

	case EMaterialState::EMS_HandHighLight:
		bLockHandHighLight = bLock;
		break;

	case EMaterialState::EMS_Death:
		bLockDeath = bLock;
		break;

	}

	EMaterialState HightestState = EMaterialState::EMS_Default;
	if (bLockDeath)
	{
		HightestState = EMaterialState::EMS_Death;
	}else if (bLockHandHighLight)
	{
		HightestState = EMaterialState::EMS_HandHighLight;
	}
	else if (bLockDamage)
	{
		HightestState = EMaterialState::EMS_Damage;
	}
	else if (bLockOnStun)
	{
		HightestState = EMaterialState::EMS_Stun;
	}
	else if (bLockOnBoardHighLight)
	{
		HightestState = EMaterialState::EMS_OnBoardHighLight;
	}


	switch (HightestState)
	{
	case EMaterialState::EMS_Default :
		ChangeMaterialEMS_Default();
		break;

	case EMaterialState::EMS_OnBoardHighLight:
		ChangeMaterialEMS_OnBoardHighLight();
		break;

	case EMaterialState::EMS_Stun:
		ChangeMaterialEMS_Stun();
		break;

	case EMaterialState::EMS_Damage:
		ChangeMaterialEMS_Damage();
		break;

	case EMaterialState::EMS_HandHighLight:

		ChangeMaterialEMS_HandHighLight();
		break;
		
	case EMaterialState::EMS_Death:
		ChangeMaterialEMS_Death();
		break;

	} 
}

void AXR_Character::ChangeMaterialEMS_Default()
{
	if (DefaultSkeletalMaterialFirst) CharacterMesh->SetMaterial(0, DefaultSkeletalMaterialFirst);
	if (DefaultSkeletalMaterialSecond) CharacterMesh->SetMaterial(1, DefaultSkeletalMaterialSecond);

}

void AXR_Character::ChangeMaterialEMS_OnBoardHighLight()
{
	if (HighlightMaterial)
	{
		CharacterMesh->SetMaterial(0, HighlightMaterial);
		CharacterMesh->SetMaterial(1, HighlightMaterial);
	}

}

void AXR_Character::ChangeMaterialEMS_Stun()
{
	if (StunMaterial)
	{
		CharacterMesh->SetMaterial(0, StunMaterial);
		CharacterMesh->SetMaterial(1, StunMaterial);
	}

}

void AXR_Character::ChangeMaterialEMS_Damage()
{
	if (DamagedMaterial)
	{
		CharacterMesh->SetMaterial(0, DamagedMaterial);
		CharacterMesh->SetMaterial(1, DamagedMaterial);

	}

}

void AXR_Character::ChangeMaterialEMS_HandHighLight()
{
	if (HasAuthority())
	{

	}
	else
	{

	}

	if (bDisableInteractable)
	{

		if (DisableHighlightMaterial)
		{
			CharacterMesh->SetMaterial(0, DisableHighlightMaterial);
			CharacterMesh->SetMaterial(1, DisableHighlightMaterial);
		}
	}
	else
	{
		if (HighlightMaterial)
		{
			CharacterMesh->SetMaterial(0, HighlightMaterial);
			CharacterMesh->SetMaterial(1, HighlightMaterial);
		}
	}
}

void AXR_Character::ChangeMaterialEMS_Death()
{

}

void AXR_Character::PlayAnimMontageMulti_Implementation(USkeletalMeshComponent* skeletalComponent, UAnimMontage* montage)
{
	if (skeletalComponent && montage)
	{
		skeletalComponent->GetAnimInstance()->Montage_Play(montage);
	}
}

void AXR_Character::CharacterActionStart()
{
	if (CharacterActionMontage && GetMesh()->GetAnimInstance())
	{
		SetAnimState(EAnimationState::EAS_Action);
		PlayAnimMontageMulti(GetMesh(), CharacterActionMontage);
	}
}


