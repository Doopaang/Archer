#include "Arrow.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "UnitComponent.h"

AArrow::AArrow()
{
	collision = CreateDefaultSubobject<USphereComponent>(TEXT("collision"));
	RootComponent = collision;
	collision->InitSphereRadius(5.0f);
	collision->BodyInstance.SetCollisionProfileName(TEXT("Arrow"));
	collision->SetCollisionProfileName(TEXT("Arrow"));

	mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("mesh"));
	mesh->SetupAttachment(RootComponent);
	mesh->GetComponentTransform().TransformPosition(FVector(30.0, 0.0f, 0.0f));
	mesh->GetComponentTransform().TransformRotation(FQuat(FVector(0.0, 1.0f, 0.0f), 90.0f));
	mesh->SetSimulatePhysics(false);
	mesh->SetCollisionProfileName(TEXT("NoCollision"));

	projectile = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile"));
	projectile->SetUpdatedComponent(collision);
	projectile->InitialSpeed = 5000.0f;
	projectile->MaxSpeed = 5000.0f;
	projectile->bRotationFollowsVelocity = true;
	projectile->ProjectileGravityScale = 0.5f;

	InitialLifeSpan = 5.0f;
}

void AArrow::Init(FVector ShootDirection)
{
	projectile->Velocity = ShootDirection * projectile->InitialSpeed;
}

void AArrow::NotifyHit(class UPrimitiveComponent* MyComp, AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);
	ReceiveHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);

	// 화살끼리 / 투명 벽일 경우 무시
	if (IsHit || Other == this || Other->Tags.Contains(TEXT("Blocking")))
	{
		return;
	}
	IsHit = true;

	// 더이상 날아가지 않음
	projectile->DestroyComponent();
	RootComponent->ComponentVelocity = FVector::ZeroVector;

	// 객체에 합쳐짐
	AttachToActor(Other, FAttachmentTransformRules::KeepWorldTransform);

	UUnitComponent* unit = Cast<UUnitComponent>(Other->GetComponentByClass(UUnitComponent::StaticClass()));
	if (unit && Other->Tags.Contains(TEXT("CanDamaged")))
	{
		unit->HitDamage(Damage);
	}

	Hit_BP(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit); // Hit 이후 BP 커스터마이징을 위한 가능성
}