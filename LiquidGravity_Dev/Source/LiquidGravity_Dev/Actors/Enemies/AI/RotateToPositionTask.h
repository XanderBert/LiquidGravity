#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "RotateToPositionTask.generated.h"


UCLASS()
class LIQUIDGRAVITY_DEV_API URotateToPositionTask : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
public:
	URotateToPositionTask();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
private:
	UPROPERTY()
	APawn* Bot{};
	FRotator StartRotation{};
	FRotator TargetRotation{};


	UPROPERTY(EditAnywhere)
	float Tolerance = 3.0f;

	UPROPERTY(EditAnywhere)
	float SlerpSpeed = 0.1f;
};