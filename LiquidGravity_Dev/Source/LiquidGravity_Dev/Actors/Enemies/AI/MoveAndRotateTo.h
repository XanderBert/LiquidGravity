#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_MoveTo.h"
#include "MoveAndRotateTo.generated.h"


UCLASS()
class LIQUIDGRAVITY_DEV_API UMoveAndRotateTo : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
public:
	UMoveAndRotateTo();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

private:
	UPROPERTY()
	AActor* Target{};
	UPROPERTY()
	APawn* Bot{};
};
