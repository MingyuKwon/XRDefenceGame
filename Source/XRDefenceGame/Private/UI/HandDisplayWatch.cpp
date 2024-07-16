
#include "UI/HandDisplayWatch.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/WidgetComponent.h"

AHandDisplayWatch::AHandDisplayWatch()
{
	PrimaryActorTick.bCanEverTick = true;

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("RootCollision"));
	RootComponent = BoxComponent;

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WatchBoard"));
	StaticMeshComponent->SetupAttachment(BoxComponent);

	UserWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("UI"));
	UserWidgetComponent->SetupAttachment(StaticMeshComponent);
}

void AHandDisplayWatch::BeginPlay()
{
	Super::BeginPlay();
}

void AHandDisplayWatch::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}