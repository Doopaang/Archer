#include "HPBar.h"
#include "Components/WidgetComponent.h"
#include "UnitComponent.h"

AHPBar::AHPBar()
{
	PrimaryActorTick.bCanEverTick = true;

	widget = CreateDefaultSubobject<UWidgetComponent>(TEXT("Widget"));
	widget->SetupAttachment(RootComponent);
	widget->SetWidgetSpace(EWidgetSpace::Screen);
	widget->SetDrawSize(FVector2D(120.0f, 20.0f));

	InitialLifeSpan = 5.0f;
}

void AHPBar::Init(UUnitComponent* newOwner, FVector newRelativePos)
{
	OwnerActor = newOwner;
	RelativePos = newRelativePos;
}

void AHPBar::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (OwnerActor && OwnerActor->HPBar)
	{
		// 위젯으로의 값 전달은 블루프린트로 처리
		UpdateBar();

		// 대상 액터를 쫒아다님
		SetActorLocation(OwnerActor->GetOwner()->GetActorLocation() + RelativePos);
	}
}

void AHPBar::BeginDestroy()
{
	Super::BeginDestroy();

	if (OwnerActor)
	{
		OwnerActor->HPBar = nullptr;
	}
}
