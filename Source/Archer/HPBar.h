#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HPBar.generated.h"

UCLASS(config = Game)
class ARCHER_API AHPBar : public AActor
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly)
	class UUnitComponent* OwnerActor;
	FVector RelativePos;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UWidgetComponent* widget;

	AHPBar();

	UFUNCTION(BlueprintCallable, Category = Unit)
	void Init(class UUnitComponent* newOwner, FVector newRelativePos);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = Unit)
	void UpdateBar();

public:	
	virtual void Tick(float DeltaTime) override;
	virtual void BeginDestroy() override;
};
