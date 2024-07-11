#include "Battle/Projectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GameFramework/Character.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/BoxComponent.h"

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


void AProjectile::SetTarget(AActor* Target)
{
    if (Projectile_Movement && Target)
    {
        UPrimitiveComponent* TargetComponent = Cast<UPrimitiveComponent>(Target->GetRootComponent());
        if (TargetComponent)
        {

        }
    }
}

void AProjectile::SetDamage(float Damage)
{
    BulletDamage = Damage;
}

void AProjectile::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    UE_LOG(LogTemp, Warning, TEXT("OnHit"));

    if (OtherActor && (OtherActor != this) && OtherComp)
    {
        if (HitImpactParticle)
        {
            UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), HitImpactParticle, GetActorLocation());
        }

        UGameplayStatics::ApplyDamage(OtherActor, BulletDamage, GetInstigatorController(), this, nullptr);

        Destroy();
    }
}

void AProjectile::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}
