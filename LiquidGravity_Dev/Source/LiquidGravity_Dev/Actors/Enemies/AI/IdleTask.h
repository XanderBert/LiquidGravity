#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "IdleTask.generated.h"

UCLASS()
class LIQUIDGRAVITY_DEV_API UIdleTask : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
public:
	UIdleTask();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

private:
	UPROPERTY(EditAnywhere)
	float IdleTime = 2.0f;
	float CurrentIdleTime = 0.0f;
};
