// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Waypoint.generated.h"

class UEnemeyWaypointComponent;

UCLASS()
class LIQUIDGRAVITY_DEV_API AWaypoint : public AActor
{
	GENERATED_BODY()
public:
	AWaypoint();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UEnemeyWaypointComponent* EnemyWaypointComponent;
};
