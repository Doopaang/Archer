#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Arrow.generated.h"

UCLASS(config = Game)
class ARCHER_API AArrow : public AActor
{
	GENERATED_BODY()

	bool IsHit = false;

	UPROPERTY(EditAnywhere)
	int Damage = 10;

	UPROPERTY(VisibleAnywhere)
	class USphereComponent* collision;

	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent* mesh;

	UPROPERTY(VisibleAnywhere)
	class UProjectileMovementComponent* projectile;

	virtual void NotifyHit(class UPrimitiveComponent* MyComp, AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;

public:
	AArrow();

	void Init(FVector ShootDirection);

	UFUNCTION(BlueprintImplementableEvent, Category = Arrow)
	void Hit_BP(class UPrimitiveComponent* MyComp, AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit);
};
