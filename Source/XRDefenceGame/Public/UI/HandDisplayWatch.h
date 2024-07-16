#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HandDisplayWatch.generated.h"

UCLASS()
class XRDEFENCEGAME_API AHandDisplayWatch : public AActor
{
	GENERATED_BODY()

public:
	AHandDisplayWatch();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(VisibleAnywhere, Category = "Components")
	class UBoxComponent* BoxComponent;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	class UStaticMeshComponent* StaticMeshComponent;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	class UWidgetComponent* UserWidgetComponent;
};
