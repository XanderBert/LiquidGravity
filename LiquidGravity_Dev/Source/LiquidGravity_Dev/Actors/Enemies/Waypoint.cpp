#include "Waypoint.h"
#include "LiquidGravity_Dev/Components/EnemeyWaypointComponent.h"

AWaypoint::AWaypoint()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	EnemyWaypointComponent = CreateDefaultSubobject<UEnemeyWaypointComponent>(TEXT("EnemyWaypointComponent"));
}


