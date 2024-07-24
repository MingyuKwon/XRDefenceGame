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

AXR_Character::AXR_Character()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	FromPaletteToCharacter = CreateDefaultSubobject<UNiagaraComponent>(FName("FromPaletteToCharacter"));
	FromPaletteToCharacter->SetupAttachment(RootComponent);

	FromCharacterToRing = CreateDefaultSubobject<UNiagaraComponent>(FName("FromCharacterToRing"));
	FromCharacterToRing->SetupAttachment(RootComponent);

	BuffRing = CreateDefaultSubobject<UNiagaraComponent>(FName("BuffRing"));
	BuffRing->SetupAttachment(GetMesh());

	HealRing = CreateDefaultSubobject<UNiagaraComponent>(FName("HealRing"));
	HealRing->SetupAttachment(GetMesh());

	SpeedBuffNiagara = CreateDefaultSubobject<UNiagaraComponent>(FName("SpeedBuffNiagara"));
	SpeedBuffNiagara->SetupAttachment(GetMesh());

	

	MotionWarpingComponent = CreateDefaultSubobject<UMotionWarpingComponent>(FName("MotionWarping"));

	FloorRingMesh = CreateDefaultSubobject<UFloorRingSMC>(FName("FloorRingMesh"));
	FloorRingMesh->SetupAttachment(RootComponent);

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

		SpawnCharacterPropertyUI();
		FloorRingMesh->bCharacterOnBoard = true;

		PlaySoundViaManager(EGameSoundType::EGST_SFX, SoundSpawnBoard, GetActorLocation(), 0.7f);

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
	GameInstance = Cast<UXRDefenceGameInstance>(GetWorld()->GetGameInstance());

	if (!GetCharacterMesh()) return;

	DefaultSkeletalMaterialFirst = Cast<UMaterialInstance>(CharacterMesh->GetMaterial(0));
	DefaultSkeletalMaterialSecond = Cast<UMaterialInstance>(CharacterMesh->GetMaterial(1));

	XRGamePlayMode = Cast<AXRGamePlayMode>(UGameplayStatics::GetGameMode(this));
	if (XRGamePlayMode)
	{
		XRGamePlayMode->OnChrarcterDieEvent.AddDynamic(this, &AXR_Character::TargetDieCallBack);
		XRGamePlayMode->OnCharacterSpawnEvent.AddDynamic(this, &AXR_Character::OtherCharacterSpawnCallBack);
		XRGamePlayMode->OnGameEnd.AddDynamic(this, &AXR_Character::GameEndCallBack);
	}

	HealRing->Deactivate();
	BuffRing->Deactivate();
	SpeedBuffNiagara->Deactivate();

	if (!DefaultPlaceInBoard)
	{
		SpawnCostShowUI();
		PlaySoundViaManager(EGameSoundType::EGST_SFX, SoundSpawnPallette, GetActorLocation(), 0.5f);

	}


	SetRingProperty();
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
	OnBoardCalledFunctionServer(true, false);

}

void AXR_Character::SpawnCostShowUI()
{
	if (costShowUIClass)
	{
		FVector SpawnLocation = GetActorLocation() - GetActorForwardVector() * 4;
		FRotator SpawnRotation = GetActorRotation();


		if (GetMesh())
		{
			SpawnLocation.Z = GetMesh()->GetComponentLocation().Z + 0.25f;
		}

		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = GetInstigator();

		CostShowUI = GetWorld()->SpawnActor<ACostShowChip>(costShowUIClass, SpawnLocation, SpawnRotation, SpawnParams);
		if (CostShowUI)
		{
			CostShowUI->SetGoldCostCount(CharacterProperty.Cost);
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

		if(!HasAuthority()) UE_LOG(LogTemp, Warning, TEXT("%s Debug SpawnCharacterPropertyUI"), *CharacterPropertyUI->GetName())
		UpdateCharacterPropertyUI();

	}
}

void AXR_Character::UpdateCharacterPropertyUI()
{
	if (HasAuthority()) MulticastUpdateCharacterPropertyUI();

}

void AXR_Character::MulticastUpdateCharacterPropertyUI_Implementation()
{
	if (CharacterPropertyUI)
	{
		if(!HasAuthority())	UE_LOG(LogTemp, Display, TEXT("MulticastUpdateCharacterPropertyUI_Implementation In Client"));

		CharacterPropertyUI->SetHealthPercent(CharacterProperty.currentHealth / CharacterProperty.MaxHealth);
		CharacterPropertyUI->SetDamageCount(CharacterProperty.currentDamage);
		CharacterPropertyUI->SetUtilCount(CharacterProperty.Util_Fast);
	}
}


void AXR_Character::SetPropertyUIVisible(bool flag)
{
	if (!CharacterPropertyUI) return;

	if (CharacterType == ECharacterType::ECT_DefenceNexus)
	{
		CharacterPropertyUI->SetDamgeUtilVisible(false);
		return;
	}


	if (ObjectType == EObjectType::EOT_Deffence && CharacterType != ECharacterType::ECT_DefenceP)
	{
		CharacterPropertyUI->SetDamgeUtilVisible(true);
		return;
	}

	if (CharacterType == ECharacterType::ECT_DefenceP || CharacterType == ECharacterType::ECT_OffenceS)
	{
		flag = false;
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

	PlaySoundViaManager(EGameSoundType::EGST_SFX, SoundHighLight, GetActorLocation(), 1.0f);

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

	bHightLighting = false;

	SetPropertyUIVisible(false);

	ChangeMaterialState(EMaterialState::EMS_HandHighLight,false);

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

	OnBoardCalledFunctionServer(isOnBoard, true);

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

void AXR_Character::TriggerHealEffect()
{
	HealRing->Deactivate();
	HealRing->Activate(true);
}

void AXR_Character::TriggerBuffEffect()
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

void AXR_Character::Death(bool bDieInTrash)
{
	if (HasAuthority())
	{
		MulticastDeath(bDieInTrash);

		if (bDieInTrash)
		{
			XRGamePlayMode->OnCostEvent.Broadcast(ObjectType, 2);
		}
		else
		{
			XRGamePlayMode->OnChrarcterDieEvent.Broadcast(this);

		}
	}


}

void AXR_Character::MulticastDeath_Implementation(bool bDieInTrash)
{
	FloorRingMesh->bTickReject = true;
	bOnBoard = false;
	bBehaviorAvailable = false;

	StartDissolveTimeline(false);
	GetWorld()->GetTimerManager().SetTimer(DeathTimerHandle, this, &AXR_Character::DeathTimerFunction, 2.0f, false);


	SetAnimState(EAnimationState::EAS_Death);
	if (CharacterDeathMontage)
	{
		GetMesh()->GetAnimInstance()->Montage_Play(CharacterDeathMontage);
	}

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
}


void AXR_Character::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AXR_Character, AnimState);
	DOREPLIFETIME(AXR_Character, CharacterProperty);

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

void AXR_Character::SetbDisableInteractable(bool flag)
{
	if (bOnBoard) return;
	if (bDisableInteractable == flag) return;

	bDisableInteractable = flag;

	SetTrashEffect(bDisableInteractable);
}

void AXR_Character::SetTrashEffect(bool flag, bool onlyNiagara)
{
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

float AXR_Character::GetCost_Implementation()
{
	return CharacterProperty.Cost;
}

void AXR_Character::SetDisableHighLight_Implementation(bool bDisable)
{
	SetbDisableInteractable(bDisable);
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
	if (HasAuthority()) MultiCharacterActionCall();

}

void AXR_Character::MultiCharacterActionCall_Implementation()
{
	if (AnimState <= EAnimationState::EAS_IdleAndWalk)
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
		TakeDamageMulticast(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	}

	return DamageAmount;

}

void AXR_Character::TakeDamageMulticast_Implementation(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (CharacterProperty.currentHealth == 0) return;

	CharacterProperty.currentHealth -= DamageAmount;
	CharacterProperty.currentHealth = FMath::Clamp(CharacterProperty.currentHealth, 0.f, CharacterProperty.MaxHealth);

	UpdateCharacterPropertyUI();

	PlaySoundViaManager(EGameSoundType::EGST_SFX, SoundDamaged, GetActorLocation(), 0.1f);


	ChangeMaterialState(EMaterialState::EMS_Damage, true);
	GetWorld()->GetTimerManager().SetTimer(DamageTimerHandle, this, &AXR_Character::DamageTimerFunction, 0.15f, false);

	if (CharacterProperty.currentHealth <= 0)
	{
		Death(false);
	}

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

void AXR_Character::MoveSpeedUp(bool bEffectOn)
{
	if (bEffectOn)
	{
		SpeedBuffNiagara->Deactivate();
		SpeedBuffNiagara->SetVariableLinearColor(FName("BodyColor"), FLinearColor::Blue);
		SpeedBuffNiagara->Activate(true);
	}

	CharacterMovementComponent->MaxWalkSpeed = CharacterMovementComponent->MaxWalkSpeed + 2.f;
}

void AXR_Character::MoveSpeedDown(bool bEffectOn)
{
	if (bEffectOn)
	{
		SpeedBuffNiagara->Deactivate();
		SpeedBuffNiagara->SetVariableLinearColor(FName("BodyColor"), FLinearColor::Red);
		SpeedBuffNiagara->Activate(true);
	}

	CharacterMovementComponent->MaxWalkSpeed = CharacterMovementComponent->MaxWalkSpeed - 2.f;
}


void AXR_Character::PlaySoundViaManager(EGameSoundType soundType, USoundBase* Sound, FVector Location, float VolumeScale)
{
	if (GameInstance)
	{
		AudioManager = (AudioManager == nullptr) ? GameInstance->GetAudioManagerSubsystem() : AudioManager;
		if (AudioManager)
		{
			AudioManager->PlaySound(soundType, Sound, Location, (SoundHighLight == Sound) ? VolumeScale :(VolumeScale * OwnVolumeScale));
		}
	}
}

void AXR_Character::ChangeMaterialState(EMaterialState materialState, bool bLock)
{
	
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



void AXR_Character::CharacterActionStart()
{
	if (CharacterActionMontage && GetMesh()->GetAnimInstance())
	{
		SetAnimState(EAnimationState::EAS_Action);
		GetMesh()->GetAnimInstance()->Montage_Play(CharacterActionMontage);
	}
}


