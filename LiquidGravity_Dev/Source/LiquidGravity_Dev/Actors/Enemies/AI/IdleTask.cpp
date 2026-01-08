#include "IdleTask.h"

UIdleTask::UIdleTask()
{
	bNotifyTick = true;
	bCreateNodeInstance = true;
}

void UIdleTask::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
	CurrentIdleTime += DeltaSeconds;

	if(CurrentIdleTime >= IdleTime)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}

EBTNodeResult::Type UIdleTask::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);
	CurrentIdleTime = 0.0f;
	return EBTNodeResult::InProgress;	
}
