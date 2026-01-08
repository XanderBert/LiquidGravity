#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "ChasePlayerTask.generated.h"

class AEnemyAiController;
/**
 * 
 */
UCLASS()
class LIQUIDGRAVITY_DEV_API UChasePlayerTask : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
public:

	UChasePlayerTask();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;


private:
	UPROPERTY()
	AActor* Player;

	UPROPERTY()
	AEnemyAiController* AiController{};
	
	UPROPERTY()
	APawn* Bot{};

	UPROPERTY()
	UBlackboardComponent* BlackboardComponent{};

	UPROPERTY(EditAnywhere)
	float HeightToHoverAbovePlayer = 100.0f;
	
	UPROPERTY(EditAnywhere)
	float BotRadiusToCheck = 75.f;

	UPROPERTY(EditAnywhere)
	float ChaseDistance = 500.0f;

	UPROPERTY(EditAnywhere)
	float DistanceFromPlayer = 150.0f;

	UPROPERTY(EditAnywhere)
	float TimeToMoveToTarget = 1.0f;

	UPROPERTY(EditAnywhere)
	float TimeToRotate = 0.3f;

	UPROPERTY(EditAnywhere)
	float TimeWithNoLineOfSightUnitlItLosesInterest = 2.6f;

	
	FVector Target{};


	void ResetBlackboard();
};
