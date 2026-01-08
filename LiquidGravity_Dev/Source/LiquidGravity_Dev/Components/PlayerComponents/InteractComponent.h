#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractComponent.generated.h"


class ALiquidPlayer;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LIQUIDGRAVITY_DEV_API UInteractComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UInteractComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void Interact() const;

	
private:
	
	UPROPERTY()
	ALiquidPlayer* LiquidPlayer{};

	UPROPERTY()
	AActor* HighlightedActor{};
};
