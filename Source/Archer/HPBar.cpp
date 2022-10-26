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
		// ���������� �� ������ �������Ʈ�� ó��
		UpdateBar();

		// ��� ���͸� �i�ƴٴ�
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
