#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "ShootingTask.generated.h"

UCLASS()
class LIQUIDGRAVITY_DEV_API UShootingTask : public UBTTask_BlackboardBase
{
	UShootingTask();
	
	
	GENERATED_BODY()
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
