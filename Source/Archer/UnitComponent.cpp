#include "UnitComponent.h"
#include "HPBar.h"

UUnitComponent::UUnitComponent()
{
	bWantsInitializeComponent = true;
}

void UUnitComponent::InitializeComponent()
{
	Super::InitializeComponent();
}

void UUnitComponent::HitDamage(int Damage)
{
	SpawnHPBar();

	HP -= Damage;
	if (HP <= 0) {
		Die();
	}
}

void UUnitComponent::Die()
{
	IsDied = true;
	HP = 0;

	if (HPBar)
	{
		HPBar->Destroy();
	}
	TArray<AActor*> array;
	GetOwner()->GetAttachedActors(array);
	for (AActor* actor : array)
	{
		actor->Destroy();
	}
	GetOwner()->Destroy();
}

void UUnitComponent::SpawnHPBar()
{
	if (!HPBarClass || HPBar)
	{
		return;
	}

	HPBar = GetWorld()->SpawnActor<AHPBar>(HPBarClass);
	if (HPBar)
	{
		HPBar->Init(this, HPBarPos);
	}
}