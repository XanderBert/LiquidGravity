// Fill out your copyright notice in the Description page of Project Settings.


#include "WaypointUsageComponent.h"

#include "EnemeyWaypointComponent.h"
#include "Components/CapsuleComponent.h"
#include "LiquidGravity_Dev/Actors/Enemies/BaseEnemy.h"
#include "LiquidGravity_Dev/Actors/Enemies/Waypoint.h"


UWaypointUsageComponent::UWaypointUsageComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UWaypointUsageComponent::BeginPlay()
{
	Super::BeginPlay();

	if(IsValid(SelectedWaypoint.Get()))
	{
		SetRandomStartWaypoint();
	}
}

FVector UWaypointUsageComponent::CycleWaypoint(bool hitTestGround)
{
	if(!IsValid(SelectedWaypoint.Get()))
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("WaypointUsageComponent: %s has no selected waypoint"), *name));
		UE_LOG(LogTemp, Error, TEXT("WaypointUsageComponent: %s has no selected waypoint -> Call SetRandomStartWaypoint yourself" ), *GetOwner()->GetName());
	}

	
	CurrentWaypointIndex = (CurrentWaypointIndex + 1) % Waypoints.Num();
	FVector wayPoint = Waypoints[CurrentWaypointIndex];

	//Raycast to ground to get the correct height taking into account the half height of the enemy
	if(hitTestGround)
	{
		FHitResult hitResult;
		FCollisionQueryParams collisionParams;
		collisionParams.AddIgnoredActor(GetOwner());
		if(GetWorld()->LineTraceSingleByChannel(hitResult, wayPoint, (wayPoint - FVector::DownVector * 100) - FVector::UpVector * 100, ECC_Visibility, collisionParams))
		{
			float halfheight = Cast<ABaseEnemy>(GetOwner())->CapsuleComponent->GetScaledCapsuleHalfHeight();

			//Use an offset so it doesn't clip into the ground
			constexpr float offset = 1.5f;
			wayPoint = hitResult.Location + FVector::UpVector * (halfheight - offset);
		}
	}
	

	
	return wayPoint;
}

void UWaypointUsageComponent::SetRandomStartWaypoint()
{
	Waypoints = SelectedWaypoint.Get()->EnemyWaypointComponent->Waypoints;

	//Random between 0 and the number of waypoints
	CurrentWaypointIndex = FMath::RandRange(0, Waypoints.Num() - 1);
}

