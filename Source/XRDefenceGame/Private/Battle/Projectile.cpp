#include "Battle/Projectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GameFramework/Character.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/BoxComponent.h"
#include "Character/Deffence/XR_CharacterDeffence.h"
#include "XRDefenceGame/XRDefenceGame.h"
#include "Kismet/GameplayStatics.h"
#include "Component/NotHitSelf_PMC.h"
#include "Sound/SoundBase.h"
#include "Particles/ParticleSystem.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "DrawDebugHelpers.h"  


// Sets default values
AProjectile::AProjectile()
{
    PrimaryActorTick.bCanEverTick = true;

    box = CreateDefaultSubobject<UBoxComponent>(FName("Root"));
    RootComponent = box;


    Capsule = CreateDefaultSubobject<UCapsuleComponent>(FName("Capsule collision"));
    Capsule->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    Capsule->SetCollisionObjectType(ECC_Bullet);
    Capsule->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
    Capsule->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
    Capsule->SetGenerateOverlapEvents(true);
    Capsule->SetupAttachment(RootComponent);

    BulletMesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("Bullet Mesh"));
    BulletMesh->SetupAttachment(Capsule);
    BulletMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    BulletMesh2 = CreateDefaultSubobject<UStaticMeshComponent>(FName("Bullet Mesh2"));
    BulletMesh2->SetupAttachment(Capsule);
    BulletMesh2->SetCollisionEnabled(ECollisionEnabled::NoCollision);



    Projectile_Movement = CreateDefaultSubobject<UNotHitSelf_PMC>(FName("NoProjectile Movement"));
    Projectile_Movement->bIsHomingProjectile = false;
    Projectile_Movement->HomingAccelerationMagnitude = 20.f; 

    Projectile_Movement->bRotationFollowsVelocity = true;
    Projectile_Movement->MaxSpeed = 10000.f;
    Projectile_Movement->InitialSpeed = 100.f;
    Projectile_Movement->ProjectileGravityScale = 0.f;



    BulletDamage = 10;

}

void AProjectile::BeginPlay()
{
    Super::BeginPlay();
    SetLifeSpan(5.f);
}


void AProjectile::SetTarget(FVector TargetPosition)
{
    if (Projectile_Movement)
    {
        explodePosition = TargetPosition;
    }
}

void AProjectile::SetDamage(float Damage)
{
    BulletDamage = Damage;
}

void AProjectile::SetDamageRadius(float radius)
{
    damageRadius = radius;
}

void AProjectile::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{

}

void AProjectile::Explode()
{
    if (HitImpactParticle)
    {
        UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), HitImpactParticle, GetActorLocation());
    }

    TArray<AActor*> IgnoreActor;

    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AXR_CharacterDeffence::StaticClass(), IgnoreActor);


    UGameplayStatics::ApplyRadialDamage(
        GetWorld(),
        BulletDamage,
        GetActorLocation(),
        damageRadius,
        UDamageType::StaticClass(),
        IgnoreActor,
        this,
        GetInstigatorController(),
        true
    );

    DrawDebugSphere(GetWorld(), GetActorLocation(), damageRadius, 24, FColor::Red, false, 2.0f, 0, 0.1f);

    Destroy();
}

void AProjectile::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (FVector::Dist(GetActorLocation(), explodePosition) <= 1.f && !bExplode)
    {
        bExplode = true;

        Explode();
    }
}
