#include "ArcherCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"

#include "Arrow.h"
#include "UnitComponent.h"
#include "HPBar.h"

//////////////////////////////////////////////////////////////////////////
// AArcherCharacter

AArcherCharacter::AArcherCharacter()
{
	GetCapsuleComponent()->InitCapsuleSize(35.f, 87.0f);

	USkeletalMeshComponent* mesh = GetMesh();
	mesh->GetComponentTransform().TransformPosition(FVector(0.0f, 0.0f, -87.0f));
	mesh->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -97.0f), FRotator(0.0f, 270.0f, 0.0f));

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = DefaultRotationRate;
	GetCharacterMovement()->MaxWalkSpeed = MoveSpeed;
	GetCharacterMovement()->JumpZVelocity = 500.f;
	GetCharacterMovement()->AirControl = 0.2f;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = DefaultArm;
	CameraBoom->SocketOffset = DefaultSocketOffset;
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	Unit = CreateDefaultSubobject<UUnitComponent>(TEXT("Unit"));
}

//////////////////////////////////////////////////////////////////////////
// Input

void AArcherCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAxis("MoveForward", this, &AArcherCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AArcherCharacter::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AArcherCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AArcherCharacter::LookUpAtRate);

	PlayerInputComponent->BindAxis("Sprint", this, &AArcherCharacter::Sprint);

	PlayerInputComponent->BindAction("Aim", IE_Pressed, this, &AArcherCharacter::Aim);
	PlayerInputComponent->BindAction("Aim", IE_Released, this, &AArcherCharacter::UnAim);
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AArcherCharacter::Fire);
}

void AArcherCharacter::TurnAtRate(float Rate)
{
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AArcherCharacter::LookUpAtRate(float Rate)
{
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AArcherCharacter::MoveForward(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AArcherCharacter::MoveRight(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, Value);
	}
}

void AArcherCharacter::Sprint(float Value)
{
	bool boolean = (Value != 0.0f) && (GetCharacterMovement()->bUseControllerDesiredRotation == false);

	GetCharacterMovement()->MaxWalkSpeed = boolean ? SprintSpeed : MoveSpeed;
}

void AArcherCharacter::Falling()
{
	Super::Falling();

	GetCharacterMovement()->RotationRate = FallingRotationRate;
}

void AArcherCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	GetCharacterMovement()->RotationRate = DefaultRotationRate;
}

void AArcherCharacter::Aim()
{
	ChangeIsAim(true);

	// 장전이 안돼있거나 장전 중이 아닐때
	FTimerManager& manager = GetWorldTimerManager();
	if (!CanAttack &&
		!manager.IsTimerActive(FireCooldownTimer))
	{
		// 화살 장전 타이머 시작
		manager.SetTimer(FireCooldownTimer, this, &AArcherCharacter::Reload, FireCooldown, true);
	}

	Aim_BP(FireCooldownTimer); // 조준 이후 BP 커스터마이징을 위한 가능성
}

void AArcherCharacter::UnAim()
{
	ChangeIsAim(false);

	CanAttack = false;

	// 장전 중일 때
	FTimerManager& manager = GetWorldTimerManager();
	if (manager.IsTimerActive(FireCooldownTimer))
	{
		manager.ClearTimer(FireCooldownTimer);
	}
}

void AArcherCharacter::ChangeIsAim(bool isAim)
{
	IsAim = isAim;

	GetCharacterMovement()->bUseControllerDesiredRotation = isAim;
	GetCharacterMovement()->bOrientRotationToMovement = !isAim;

	CameraBoom->TargetArmLength = isAim ? AimArm : DefaultArm;
	CameraBoom->SocketOffset = isAim ? AimSocketOffset : DefaultSocketOffset;
	FollowCamera->FieldOfView = isAim ? 100.0f : 90.0f;
}

void AArcherCharacter::Reload()
{
	FTimerManager& manager = GetWorldTimerManager();
	manager.ClearTimer(FireCooldownTimer);

	CanAttack = true;

	Reload_BP(); // 재장전 이후 BP 커스터마이징을 위한 가능성
}

void AArcherCharacter::Fire()
{
	if (!CanAttack)
	{
		return;
	}

	// 재장전 시작
	CanAttack = false;
	Aim();

	if (ArrowClass)
	{
		// 캐릭터 손이 아닌 카메라에서 발사
		// 기획 상 큰 차이가 없고 구현 난이도가 더 쉬우며 '카메라 방향을 따르는 에임오프셋' + '카메라의 조준점에 탄착'의 타협점
		// 3인칭을 이용해 장애물에 숨어서 카메라로 발사할 가능성이 있음
		FVector CameraLocation = FollowCamera->GetComponentLocation();
		FRotator CameraRotation = FollowCamera->GetComponentRotation();
		FVector MuzzleLocation = CameraLocation + FTransform(CameraRotation).TransformVector(MuzzleOffset);
		FRotator MuzzleRotation = CameraRotation;
		MuzzleRotation.Pitch += 0.0f;
		UWorld* World = GetWorld();
		if (World)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			SpawnParams.Instigator = GetInstigator();
			AArrow* Projectile = World->SpawnActor<AArrow>(ArrowClass, MuzzleLocation, MuzzleRotation, SpawnParams);
			if (Projectile)
			{
				FVector LaunchDirection = MuzzleRotation.Vector();
				Projectile->Init(LaunchDirection);
			}
		}
	}

	Fire_BP(); // 발사 이후 BP 커스터마이징을 위한 가능성
}
