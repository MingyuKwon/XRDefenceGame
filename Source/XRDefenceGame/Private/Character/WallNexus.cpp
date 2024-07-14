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


}

void AWallNexus::BeginPlay()
{
	Super::BeginPlay();
	
}

void AWallNexus::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

