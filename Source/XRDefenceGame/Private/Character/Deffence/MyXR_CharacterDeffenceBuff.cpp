// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Deffence/MyXR_CharacterDeffenceBuff.h"

AMyXR_CharacterDeffenceBuff::AMyXR_CharacterDeffenceBuff()
{
    RingMeshComponent1 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RingMeshComponent1"));
    RingMeshComponent1->SetupAttachment(GetMesh());
    RingMeshComponent1->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    RingMeshComponent2 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RingMeshComponent2"));
    RingMeshComponent2->SetupAttachment(GetMesh());
    RingMeshComponent2->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    RingMeshComponent3 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RingMeshComponent3"));
    RingMeshComponent3->SetupAttachment(GetMesh());
    RingMeshComponent3->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    ElapsedTime = 0.0f;
    MinZ = 0.0f;     
    MaxZ = 200.0f;    

    bMovingUp1 = true;
    bMovingUp2 = true;
    bMovingUp3 = true;


}

void AMyXR_CharacterDeffenceBuff::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    ElapsedTime += DeltaTime;

    UpdateComponentPosition(RingMeshComponent1, InitialLocation1, bMovingUp1, DeltaTime, 200.f);
    UpdateComponentPosition(RingMeshComponent2, InitialLocation2, bMovingUp2, DeltaTime, 300.f);
    UpdateComponentPosition(RingMeshComponent3, InitialLocation3, bMovingUp3, DeltaTime, 400.f);


    FRotator NewRotation(0.0f, 3.0f, 0.f);
    RingMeshComponent1->AddLocalRotation(NewRotation);
    RingMeshComponent2->AddLocalRotation(NewRotation);
    RingMeshComponent3->AddLocalRotation(NewRotation);

}

void AMyXR_CharacterDeffenceBuff::BeginPlay()
{
    Super::BeginPlay();

    if (RingMeshComponent1)
    {
        InitialLocation1 = RingMeshComponent1->GetRelativeLocation();
    }
    if (RingMeshComponent2)
    {
        InitialLocation2 = RingMeshComponent2->GetRelativeLocation();
    }
    if (RingMeshComponent3)
    {
        InitialLocation3 = RingMeshComponent3->GetRelativeLocation();
    }
}

void AMyXR_CharacterDeffenceBuff::BindDissolveCallBack()
{
    InterpFunction.BindDynamic(this, &AMyXR_CharacterDeffenceBuff::DissolveCallBack);
}

void AMyXR_CharacterDeffenceBuff::BindReverseDissolveCallBack()
{
    InterpFunction.BindDynamic(this, &AMyXR_CharacterDeffenceBuff::DissolveCallBackReverse);
}

void AMyXR_CharacterDeffenceBuff::DissolveCallBack(float percent)
{
    if (RingMeshComponent1->GetStaticMesh() != nullptr) RingMeshComponent1->SetScalarParameterValueOnMaterials("Dissolve", percent);
    if (RingMeshComponent2->GetStaticMesh() != nullptr) RingMeshComponent2->SetScalarParameterValueOnMaterials("Dissolve", percent);
    if (RingMeshComponent3->GetStaticMesh() != nullptr) RingMeshComponent3->SetScalarParameterValueOnMaterials("Dissolve", percent);

    Super::DissolveCallBack(1-percent);

}

void AMyXR_CharacterDeffenceBuff::DissolveCallBackReverse(float percent)
{
    percent = 1 - percent;
    if (RingMeshComponent1->GetStaticMesh() != nullptr) RingMeshComponent1->SetScalarParameterValueOnMaterials("Dissolve", percent);
    if (RingMeshComponent2->GetStaticMesh() != nullptr) RingMeshComponent2->SetScalarParameterValueOnMaterials("Dissolve", percent);
    if (RingMeshComponent3->GetStaticMesh() != nullptr) RingMeshComponent3->SetScalarParameterValueOnMaterials("Dissolve", percent);
}

void AMyXR_CharacterDeffenceBuff::OnBoardCalledFunction(bool isOnBoard)
{
    Super::OnBoardCalledFunction(isOnBoard);

    RingMeshComponent1->SetVisibility(isOnBoard);
    RingMeshComponent2->SetVisibility(isOnBoard);
    RingMeshComponent3->SetVisibility(isOnBoard);

    GetWorld()->GetTimerManager().SetTimer(LifeTimeTimerHandle, this, &AMyXR_CharacterDeffenceBuff::LifeTimeTimerFunction, 2.0f, false);

}

void AMyXR_CharacterDeffenceBuff::PoolSpawnBeginPlay()
{
    Super::PoolSpawnBeginPlay();

    RingMeshComponent1->SetVisibility(false);
    RingMeshComponent2->SetVisibility(false);
    RingMeshComponent3->SetVisibility(false);

}

void AMyXR_CharacterDeffenceBuff::PoolSpawnDestryoed()
{
    Super::PoolSpawnDestryoed();

    GetWorld()->GetTimerManager().ClearTimer(LifeTimeTimerHandle);
    GetMesh()->SetScalarParameterValueOnMaterials("Dissolve", 0);

}

void AMyXR_CharacterDeffenceBuff::UpdateComponentPosition(USceneComponent* Component, FVector InitialLocation, bool& bMovingUp, float DeltaTime, float MoveSpeed)
{
    if (Component)
    {
        FVector NewLocation = Component->GetRelativeLocation();
        float NewZ = NewLocation.Z + (bMovingUp ? MoveSpeed : -MoveSpeed) * DeltaTime;

        if (NewZ >= MaxZ)
        {
            NewZ = MaxZ;
            bMovingUp = false;
        }
        else if (NewZ <= MinZ)
        {
            NewZ = MinZ;
            bMovingUp = true;
        }

        NewLocation.Z = NewZ;
        Component->SetRelativeLocation(NewLocation);
    }
}

void AMyXR_CharacterDeffenceBuff::LifeTimeTimerFunction()
{
    Death();
}


