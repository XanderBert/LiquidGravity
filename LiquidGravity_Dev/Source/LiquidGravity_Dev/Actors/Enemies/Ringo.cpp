#include "Ringo.h"

#include "EnemySpawner.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
ARingo::ARingo()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void ARingo::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	
	//Get all enemy spawners and increase their modifier
	TArray<AActor*> spawners;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemySpawner::StaticClass(), spawners);
	
	for(AActor* spawner : spawners)
	{
		AEnemySpawner* enemySpawner = Cast<AEnemySpawner>(spawner);
		if(enemySpawner)
		{
			enemySpawner->SetModifier(enemySpawner->GetModifier() + 1);
		}
	}
}