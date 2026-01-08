#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WaypointUsageComponent.generated.h"


class AWaypoint;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LIQUIDGRAVITY_DEV_API UWaypointUsageComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UWaypointUsageComponent();
	virtual void BeginPlay() override;

	FVector CycleWaypoint(bool hitTestGround = false);
	
	UPROPERTY(EditAnywhere, Category = "Waypoints")
	TSoftObjectPtr<AWaypoint> SelectedWaypoint;


	TArray<FVector> Waypoints{};

	
	void SetRandomStartWaypoint();

private:
	int CurrentWaypointIndex{0};

};