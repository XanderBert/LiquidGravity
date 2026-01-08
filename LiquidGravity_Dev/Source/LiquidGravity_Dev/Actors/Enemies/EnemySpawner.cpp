#include "EnemySpawner.h"

#include "BaseEnemy.h"
#include "EngineUtils.h"
#include "Waypoint.h"
#include "LiquidGravity_Dev/Components/WaypointUsageComponent.h"


AEnemySpawner::AEnemySpawner()
{
	PrimaryActorTick.bCanEverTick = true;
}


void AEnemySpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	int aliveCount = GetAliveCount();

	//Start eh spawning if we go below the minimum amount of enemies
	if(!bShouldSpawn)
	{
		if(aliveCount < MinNumberOfEnemies * Modifier)
		{
			bShouldSpawn = true;
			SetRandomInterval();
		}
	}


	//Stop the spawning if we reach the maximum amount of enemies
	if(aliveCount >= (MaxNumberOfEnemies * Modifier))
	{
		bShouldSpawn = false;
	}
	
	
	if(bShouldSpawn)
	{
		SpawnAtRandomInterval(DeltaTime);
	}
}

void AEnemySpawner::SpawnAtRandomInterval(float deltaTime)
{
	SpawnTimer -= deltaTime;

	if(SpawnTimer <= 0.0f)
	{
		//Reset the timer
		SetRandomInterval();

		
		//Spawn the enemy
		SpawnEnemyAndSetWaypoint();
	}
}

void AEnemySpawner::SetRandomInterval()
{
	SpawnTimer = FMath::RandRange(MinSpawnInterval, MaxSpawnInterval);
}

int AEnemySpawner::GetAliveCount() const
{
	int aliveCount = 0;
	
	//TODO: Move to a event OnEnemyDeath -> Decrease the amount of enemies in the world
	//Get the amount of enemies in the world of type EnemyToSpawn
	UWorld* world = GetWorld();
	if (world && EnemyToSpawn)
	{
		for (TActorIterator<AActor> It(world, EnemyToSpawn); It; ++It)
		{
			AActor* Enemy = *It;
			if (IsValid(Enemy) && Cast<ABaseEnemy>(Enemy)->SpawnedBy == this)
			{
				aliveCount++;
			}
		}
	}


	return aliveCount;
}

void AEnemySpawner::SpawnEnemyAndSetWaypoint()
{
	//Spawn the enemy
	if(EnemyToSpawn)
	{
		FActorSpawnParameters SpawnParams{};
		SpawnParams.Owner = this;
		SpawnParams.Instigator = GetInstigator();
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

		FVector spawnLocation = GetActorLocation();
	

		AActor* enemy = GetWorld()->SpawnActor<AActor>(EnemyToSpawn, spawnLocation, FRotator::ZeroRotator, SpawnParams);
		
		if(enemy)
		{
			//Set the spawner
			Cast<ABaseEnemy>(enemy)->SpawnedBy = this;
			
			check(WaypointToUse.Num() > 0);
			
				
			//Get a rondom waypoint
			int randomIndex = FMath::RandRange(0, WaypointToUse.Num() - 1);
			//UE_LOG(LogTemp, Warning, TEXT("Random index: %d"), randomIndex);

			AWaypoint* waypoint = WaypointToUse[randomIndex];
			
			check(IsValid(waypoint));
 
			//Get the waypointUsageComponent
			UWaypointUsageComponent* waypointUsageComponent = enemy->FindComponentByClass<UWaypointUsageComponent>();
			check(waypointUsageComponent);
 
			//Set its waypoint
			waypointUsageComponent->SelectedWaypoint = waypoint;
			waypointUsageComponent->SetRandomStartWaypoint();
		}else
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("EnemySpawner: Failed to spawn enemy"));
		}
	}
}
