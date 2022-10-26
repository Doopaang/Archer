#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "UnitComponent.h"
#include "ArcherCharacter.generated.h"


UCLASS(config = Game)
class AArcherCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AArcherCharacter();

private:
	UPROPERTY(VisibleAnywhere, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;
	UPROPERTY(VisibleAnywhere, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	UPROPERTY(VisibleAnywhere, Category = Unit, meta = (AllowPrivateAccess = "true"))
	class UUnitComponent* Unit;

	const float MoveSpeed = 300.0f;
	const float SprintSpeed = 800.0f;

	const FRotator DefaultRotationRate = FRotator(0.0f, 540.0f, 0.0f);
	const FRotator FallingRotationRate = FRotator(0.0f, 0.0f, 0.0f);

	const float DefaultArm = 150.0f;
	const FVector DefaultSocketOffset = FVector(0.0f, 50.0f, 45.0f);
	const float AimArm = 0.0f;
	const FVector AimSocketOffset = FVector(-50.0f, 45.0f, 60.0f);

	FVector ArrowPosition = FVector(0.0f, 100.0f, 0.0f);

	bool CanAttack = false;
	bool IsAim = false;

	const float FireCooldown = 1.0f;
	FTimerHandle FireCooldownTimer;

	const FVector MuzzleOffset = FVector(100.0f, 0.0f, 0.0f);
	UPROPERTY(EditDefaultsOnly, Category = Player)
	TSubclassOf<class AArrow> ArrowClass;

	void ChangeIsAim(bool isAim);
	void Reload();

public:
	const float BaseTurnRate = 45.0f;
	const float BaseLookUpRate = 45.0f;

	UFUNCTION(BluePrintImplementableEvent, Category = Player)
	void Aim_BP(FTimerHandle timer);
	UFUNCTION(BluePrintImplementableEvent, Category = Player)
	void Reload_BP();

	void Fire();
	UFUNCTION(BluePrintImplementableEvent, Category = Player)
	void Fire_BP();

protected:
	void MoveForward(float Value);
	void MoveRight(float Value);
	void TurnAtRate(float Rate);
	void LookUpAtRate(float Rate);

	void Sprint(float Value);

	void Aim();
	void UnAim();

	virtual void Falling() override;
	virtual void Landed(const FHitResult& Hit) override;

protected:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};

