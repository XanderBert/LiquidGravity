#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EnemeyWaypointComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LIQUIDGRAVITY_DEV_API UEnemeyWaypointComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UEnemeyWaypointComponent();

	virtual void BeginPlay() override;
	
	


	
	UPROPERTY(EditAnywhere, Category = "Waypoints")
	TArray<FVector> Waypoints;
};