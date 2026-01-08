#include "EnemeyWaypointComponent.h"

#include "Components/CapsuleComponent.h"
#include "LiquidGravity_Dev/Actors/Enemies/BaseEnemy.h"

UEnemeyWaypointComponent::UEnemeyWaypointComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UEnemeyWaypointComponent::BeginPlay()
{
	Super::BeginPlay();
	


}


