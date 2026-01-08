#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "GetRandomLocationTask.generated.h"

UCLASS()
class LIQUIDGRAVITY_DEV_API UGetRandomLocationTask : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
public:
	UGetRandomLocationTask();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;


private:
	UPROPERTY(EditAnywhere, Category = "Waypoint", meta = (AllowPrivateAccess = "true"))
	bool SnapPointToGround = false;
};
