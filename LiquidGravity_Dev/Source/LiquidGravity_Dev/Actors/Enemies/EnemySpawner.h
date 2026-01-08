#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemySpawner.generated.h"

class AWaypoint;
class ABaseEnemy;

UCLASS()
class LIQUIDGRAVITY_DEV_API AEnemySpawner : public AActor
{
	GENERATED_BODY()
public:

	AEnemySpawner();

	
	virtual void Tick(float DeltaTime) override;

	void SetModifier(int modifier) { Modifier = modifier; }
	int GetModifier() const { return Modifier; }

private:	
	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> EnemyToSpawn;
	
	UPROPERTY(EditAnywhere)
	TArray<AWaypoint*> WaypointToUse;


	UPROPERTY(EditAnywhere)
	int MinNumberOfEnemies = 3;

	UPROPERTY(EditAnywhere)
	int MaxNumberOfEnemies = 8;


	UPROPERTY(EditAnywhere)
	float MinSpawnInterval = 1.3f;

	UPROPERTY(EditAnywhere)
	float MaxSpawnInterval = 6.0f;
	
	bool bShouldSpawn = false;

	
	float SpawnTimer = 0.0f;

	int Modifier = 1;
	
	void SpawnAtRandomInterval(float deltaTime);
	void SpawnEnemyAndSetWaypoint();
	void SetRandomInterval();

	//Get all alive enemies that where spawned by this spawner
	int GetAliveCount() const;
};