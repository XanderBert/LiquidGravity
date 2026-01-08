#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "UpdateLOFPlayerTask.generated.h"

UCLASS()
class LIQUIDGRAVITY_DEV_API UUpdateLOFPlayerTask : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
public:
	UUpdateLOFPlayerTask();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
