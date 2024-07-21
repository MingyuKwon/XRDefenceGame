// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

UCLASS()
class XRDEFENCEGAME_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:
	// Sets default values for this actor's properties
	AProjectile();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UNotHitSelf_PMC* Projectile_Movement;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* BulletMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* BulletMesh2;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UCapsuleComponent* Capsule;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UBoxComponent* box;


	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class UNiagaraSystem* HitImpactParticle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Niagara Parameter")
	class UNiagaraSystem* BombRangeNiagara;

	UPROPERTY(EditAnywhere, Category = "Sound Parameter")
	USoundBase* SoundBomb;

private:
	UPROPERTY(EditAnywhere)
	float shotSpeed;
	float BulletDamage = 0.f;

	UPROPERTY(EditAnywhere)
	float damageRadius = 5.f;

	FVector explodePosition;
	bool bExplode = false;

	UFUNCTION(BlueprintCallable)
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	void Explode();

public:
	virtual void Tick(float DeltaTime) override;
	UFUNCTION(BlueprintCallable)
	void SetTarget(FVector TargetPosition);
	UFUNCTION(BlueprintCallable)
	void SetDamage(float Damage);

};
