#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "GoToNextLocationTask.generated.h"

class AEnemyAiController;

UCLASS()
class LIQUIDGRAVITY_DEV_API UGoToNextLocationTask : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
public:
	UGoToNextLocationTask();
	
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
private:
	UPROPERTY(EditAnywhere)
	float TimeToMoveAMeter{3.0};

	UPROPERTY(EditAnywhere)
	float TimeToRotate{0.3};

	UPROPERTY(EditAnywhere)
	FVector2D ClampedPitch{-30, 30};
	
	UPROPERTY(EditAnywhere)
	float AcceptableRange{55.0f};
	
	FVector NextLocation{};

	UPROPERTY()
	APawn* Bot{};


	float Speed{};
};
