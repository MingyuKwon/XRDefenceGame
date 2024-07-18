// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/WallNexus.h"
#include "Components/BoxComponent.h"

// Sets default values
AWallNexus::AWallNexus()
{
	PrimaryActorTick.bCanEverTick = true;

	NexusMesh1 = CreateDefaultSubobject<UStaticMeshComponent>(FName("NexusMesh1"));
	NexusMesh1->SetupAttachment(GetMesh());
	NexusMesh1->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	NexusMesh2 = CreateDefaultSubobject<UStaticMeshComponent>(FName("NexusMesh2"));
	NexusMesh2->SetupAttachment(GetMesh());
	NexusMesh2->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	NexusMesh3 = CreateDefaultSubobject<UStaticMeshComponent>(FName("NexusMesh3"));
	NexusMesh3->SetupAttachment(GetMesh());
	NexusMesh3->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	NexusMesh4 = CreateDefaultSubobject<UStaticMeshComponent>(FName("NexusMesh4"));
	NexusMesh4->SetupAttachment(GetMesh());
	NexusMesh4->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	NexusMesh5 = CreateDefaultSubobject<UStaticMeshComponent>(FName("NexusMesh5"));
	NexusMesh5->SetupAttachment(GetMesh());
	NexusMesh5->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    TimeAccumulator = 0.0f;
    bMovingUp = true;

}

void AWallNexus::BeginPlay()
{
	Super::BeginPlay();
	
}

void AWallNexus::InitializeCharacter()
{
    DefaultNexusMesh1 = Cast<UMaterialInstance>(NexusMesh1->GetMaterial(0));
    DefaultNexusMesh2 = Cast<UMaterialInstance>(NexusMesh2->GetMaterial(0));
    DefaultNexusMesh3 = Cast<UMaterialInstance>(NexusMesh3->GetMaterial(0));
    DefaultNexusMesh4 = Cast<UMaterialInstance>(NexusMesh4->GetMaterial(0));
    DefaultNexusMesh5 = Cast<UMaterialInstance>(NexusMesh5->GetMaterial(0));

    Super::InitializeCharacter();

}

void AWallNexus::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

   
    FRotator NewRotation(0.0f, 0.5f, 0.0f);
    NexusMesh2->AddRelativeRotation(NewRotation);

    NewRotation = FRotator(0.0f, -0.5f, 0.0f);
    NexusMesh1->AddRelativeRotation(NewRotation);


    float MinZ = -30.0f;
    float MaxZ = 10.0f;
    float Speed = 1.0f; 

    float TargetZ = bMovingUp ? MaxZ : MinZ;

    float NewZ = FMath::FInterpTo(NexusMesh1->GetRelativeLocation().Z, TargetZ, DeltaTime, Speed);

    if (FMath::IsNearlyEqual(NewZ, TargetZ, 0.1f))
    {
        bMovingUp = !bMovingUp;
    }

    NexusMesh1->SetRelativeLocation(FVector(0.0f, 0.0f, NewZ));
}

void AWallNexus::DissolveCallBack(float percent)
{

    if (NexusMesh1->GetStaticMesh() != nullptr) {
        NexusMesh1->SetScalarParameterValueOnMaterials("Dissolve", percent);
        NexusMesh1->SetScalarParameterValueOnMaterials("Dark", 1 - percent);

    }

    if (NexusMesh2->GetStaticMesh() != nullptr) {
        NexusMesh2->SetScalarParameterValueOnMaterials("Dissolve", percent);
        NexusMesh2->SetScalarParameterValueOnMaterials("Dark", 1 - percent);

    }

    if (NexusMesh3->GetStaticMesh() != nullptr) {
        NexusMesh3->SetScalarParameterValueOnMaterials("Dissolve", percent);
        NexusMesh3->SetScalarParameterValueOnMaterials("Dark", 1 - percent);

    }

    if (NexusMesh4->GetStaticMesh() != nullptr) {
        NexusMesh4->SetScalarParameterValueOnMaterials("Dissolve", percent);
        NexusMesh4->SetScalarParameterValueOnMaterials("Dark", 1 - percent);

    }

    if (NexusMesh5->GetStaticMesh() != nullptr)
    {
        NexusMesh5->SetScalarParameterValueOnMaterials("Dissolve", percent);
        NexusMesh5->SetScalarParameterValueOnMaterials("Dark", 1 - percent);

    }

    Super::DissolveCallBack(percent);
}

void AWallNexus::DissolveCallBackReverse(float percent)
{
    Super::DissolveCallBackReverse(percent);
    DissolveCallBack(1 - percent);
}

void AWallNexus::ChangeMaterialEMS_Default()
{
    Super::ChangeMaterialEMS_Default();

    if (DefaultNexusMesh1) NexusMesh1->SetMaterial(0, DefaultNexusMesh1);
    if (DefaultNexusMesh2) NexusMesh2->SetMaterial(0, DefaultNexusMesh2);
    if (DefaultNexusMesh3) NexusMesh3->SetMaterial(0, DefaultNexusMesh3);
    if (DefaultNexusMesh4) NexusMesh4->SetMaterial(0, DefaultNexusMesh4);
    if (DefaultNexusMesh5) NexusMesh5->SetMaterial(0, DefaultNexusMesh5);

}

void AWallNexus::ChangeMaterialEMS_OnBoardHighLight()
{
    Super::ChangeMaterialEMS_OnBoardHighLight();

    if (HighlightMaterial)
    {
        NexusMesh1->SetMaterial(0, HighlightMaterial);
        NexusMesh2->SetMaterial(0, HighlightMaterial);
        NexusMesh3->SetMaterial(0, HighlightMaterial);
        NexusMesh4->SetMaterial(0, HighlightMaterial);
        NexusMesh5->SetMaterial(0, HighlightMaterial);
    }


}

void AWallNexus::ChangeMaterialEMS_Damage()
{
    Super::ChangeMaterialEMS_Damage();
    if (DamagedMaterial)
    {
        NexusMesh1->SetMaterial(0, DamagedMaterial);
        NexusMesh2->SetMaterial(0, DamagedMaterial);
        NexusMesh3->SetMaterial(0, DamagedMaterial);
        NexusMesh4->SetMaterial(0, DamagedMaterial);
        NexusMesh5->SetMaterial(0, DamagedMaterial);

    }
}

void AWallNexus::ChangeMaterialEMS_HandHighLight()
{
    Super::ChangeMaterialEMS_HandHighLight();

    if (HighlightMaterial)
    {
        NexusMesh1->SetMaterial(0, HighlightMaterial);
        NexusMesh2->SetMaterial(0, HighlightMaterial);
        NexusMesh3->SetMaterial(0, HighlightMaterial);
        NexusMesh4->SetMaterial(0, HighlightMaterial);
        NexusMesh5->SetMaterial(0, HighlightMaterial);
    }
}

void AWallNexus::ChangeMaterialEMS_Death()
{
    Super::ChangeMaterialEMS_Death();

}

