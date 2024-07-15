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

AXR_Character::AXR_Character()
{
	PrimaryActorTick.bCanEverTick = true;

	FromPaletteToCharacter = CreateDefaultSubobject<UNiagaraComponent>(FName("FromPaletteToCharacter"));
	FromPaletteToCharacter->SetupAttachment(RootComponent);

	FromCharacterToRing = CreateDefaultSubobject<UNiagaraComponent>(FName("FromCharacterToRing"));
	FromCharacterToRing->SetupAttachment(RootComponent);

	BuffRing = CreateDefaultSubobject<UNiagaraComponent>(FName("BuffRing"));
	BuffRing->SetupAttachment(GetMesh());

	HealRing = CreateDefaultSubobject<UNiagaraComponent>(FName("HealRing"));
	HealRing->SetupAttachment(GetMesh());

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
	if (bOnBoard)
	{
		StartDissolveTimeline(true);

		GetWorld()->GetTimerManager().SetTimer(BehaviorAvailableTimerHandle, this, &AXR_Character::BehaviorAvailableTimerFunction, 2.0f, false);

		SpawnCharacterPropertyUI();
		FloorRingMesh->bCharacterOnBoard = true;

		PlaySoundViaManager(EGameSoundType::EGST_SFX, SoundSpawnBoard, GetActorLocation(), 0.8f);

		if (XRGamePlayMode)
		{
			XRGamePlayMode->OnCharacterSpawnEvent.Broadcast(GetActorLocation());

		}

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
	GameInstance = Cast<UXRDefenceGameInstance>(GetWorld()->GetGameInstance());

	if (!GetCharacterMesh()) return;

	DefaultSkeletalMaterialFirst = Cast<UMaterialInstance>(CharacterMesh->GetMaterial(0));
	DefaultSkeletalMaterialSecond = Cast<UMaterialInstance>(CharacterMesh->GetMaterial(1));

	XRGamePlayMode = Cast<AXRGamePlayMode>(UGameplayStatics::GetGameMode(this));
	if (XRGamePlayMode)
	{
		XRGamePlayMode->OnChrarcterDieEvent.AddDynamic(this, &AXR_Character::TargetDieCallBack);
		XRGamePlayMode->OnCharacterSpawnEvent.AddDynamic(this, &AXR_Character::OtherCharacterSpawnCallBack);
	}

	HealRing->Deactivate();
	BuffRing->Deactivate();

	if (!DefaultPlaceInBoard)
	{
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

	PlaySoundViaManager(EGameSoundType::EGST_SFX, SoundHighLight, GetActorLocation(), 0.4f);

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

void AXR_Character::Death()
{
	FloorRingMesh->bTickReject = true;
	bOnBoard = false;
	bBehaviorAvailable = false;

	StartDissolveTimeline(false);
	GetWorld()->GetTimerManager().SetTimer(DeathTimerHandle, this, &AXR_Character::DeathTimerFunction, 2.0f, false);
	
	XRGamePlayMode->OnChrarcterDieEvent.Broadcast(this);

	PlaySoundViaManager(EGameSoundType::EGST_SFX, SoundDeath, GetActorLocation(), 0.5f);

	if (CharacterPropertyUI)
	{
		CharacterPropertyUI->Destroy();
		CharacterPropertyUI = nullptr;
	}

	SetAnimState(EAnimationState::EAS_Death);
	if (CharacterDeathMontage)
	{
		GetMesh()->GetAnimInstance()->Montage_Play(CharacterDeathMontage);
	}

	CharacterMovementComponent->MaxWalkSpeed = 0.f;


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

void AXR_Character::Heal(float healAmount)
{
	CharacterProperty.currentHealth += healAmount;
	CharacterProperty.currentHealth = FMath::Clamp(CharacterProperty.currentHealth, 0.f, CharacterProperty.MaxHealth);
	TriggerHealEffect();
	PlaySoundViaManager(EGameSoundType::EGST_SFX, SoundHeal, GetActorLocation(), 0.3f);

	UpdateCharacterPropertyUI();
}

void AXR_Character::AttackBuff(float BuffAmount)
{
	CharacterProperty.currentDamage = CharacterProperty.defaultDamage + BuffAmount;
	GetWorld()->GetTimerManager().SetTimer(BuffTimerHandle, this, &AXR_Character::BuffEndTimerFunction, BuffTime, false);
	PlaySoundViaManager(EGameSoundType::EGST_SFX, SoundBuff, GetActorLocation(), 0.3f);
	TriggerBuffEffect();
}

void AXR_Character::BuffEndTimerFunction()
{
	CharacterProperty.currentDamage = CharacterProperty.defaultDamage;
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

void AXR_Character::CharacterActionSound()
{
	PlaySoundViaManager(EGameSoundType::EGST_SFX, SoundAction, GetActorLocation(), 1.f);

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

	if (CharacterProperty.currentHealth == 0) return 0;

	CharacterProperty.currentHealth -= DamageAmount;
	CharacterProperty.currentHealth = FMath::Clamp(CharacterProperty.currentHealth, 0.f, CharacterProperty.MaxHealth);

	UpdateCharacterPropertyUI();

	PlaySoundViaManager(EGameSoundType::EGST_SFX, SoundDamaged, GetActorLocation(), 0.2f);


	ChangeMaterialState(EMaterialState::EMS_Damage, true);
	GetWorld()->GetTimerManager().SetTimer(DamageTimerHandle, this, &AXR_Character::DamageTimerFunction, 0.15f, false);

	if (CharacterProperty.currentHealth <= 0)
	{
		Death();
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

	if (state > EAnimationState::EAS_IdleAndWalk)
	{
		CharacterMovementComponent->MaxWalkSpeed = 0.f;
	}
	else
	{
		CharacterMovementComponent->MaxWalkSpeed = 5.f;
	}


}

void AXR_Character::PlaySoundViaManager(EGameSoundType soundType, USoundBase* Sound, FVector Location, float VolumeScale)
{
	if (GameInstance)
	{
		AudioManager = (AudioManager == nullptr) ? GameInstance->GetAudioManagerSubsystem() : AudioManager;
		if (AudioManager)
		{
			AudioManager->PlaySound(soundType, Sound, Location, VolumeScale * OwnVolumeScale);
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
	}
	else if (bLockHandHighLight)
	{
		HightestState = EMaterialState::EMS_HandHighLight;
	}
	else if (bLockDamage)
	{
		HightestState = EMaterialState::EMS_Damage;
	}
	else if (bLockOnBoardHighLight)
	{
		HightestState = EMaterialState::EMS_OnBoardHighLight;
	}


	switch (HightestState)
	{
	case EMaterialState::EMS_Default :
		if (DefaultSkeletalMaterialFirst) CharacterMesh->SetMaterial(0, DefaultSkeletalMaterialFirst);
		if (DefaultSkeletalMaterialSecond) CharacterMesh->SetMaterial(1, DefaultSkeletalMaterialSecond);

		break;

	case EMaterialState::EMS_OnBoardHighLight:
		if (HighlightMaterial)
		{
			CharacterMesh->SetMaterial(0, HighlightMaterial);
			CharacterMesh->SetMaterial(1, HighlightMaterial);
		}
		break;

	case EMaterialState::EMS_Damage:

		if (DamagedMaterial)
		{
			CharacterMesh->SetMaterial(0, DamagedMaterial);
			CharacterMesh->SetMaterial(1, DamagedMaterial);

		}
		break;

	case EMaterialState::EMS_HandHighLight:
		if (HighlightMaterial)
		{
			CharacterMesh->SetMaterial(0, HighlightMaterial);
			CharacterMesh->SetMaterial(1, HighlightMaterial);
		}

		break;
		
	case EMaterialState::EMS_Death:
		break;

	} 
}


void AXR_Character::CharacterActionStart()
{
	if (CharacterActionMontage && GetMesh()->GetAnimInstance())
	{
		SetAnimState(EAnimationState::EAS_Action);
		GetMesh()->GetAnimInstance()->Montage_Play(CharacterActionMontage);
	}
}

