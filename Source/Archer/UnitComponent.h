#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "UnitComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class UUnitComponent : public UActorComponent
{
	GENERATED_BODY()

	bool IsDied = false;

	void SpawnHPBar();

public:
	UUnitComponent();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Unit)
	int MaxHP;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Unit)
	int HP;

	UPROPERTY(EditAnywhere, Category = Unit)
	struct FVector HPBarPos;

	UPROPERTY(EditAnywhere, Category = Unit)
	class TSubclassOf<class AHPBar> HPBarClass;
	UPROPERTY(BlueprintReadWrite, Category = Unit)
	class AHPBar* HPBar;

	UFUNCTION(BlueprintCallable, Category = Unit)
	void HitDamage(int Damage);
	UFUNCTION(BlueprintCallable, Category = Unit)
	void Die();
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = Unit)
	void Die_BP();

protected:
	virtual void InitializeComponent() override;
};
