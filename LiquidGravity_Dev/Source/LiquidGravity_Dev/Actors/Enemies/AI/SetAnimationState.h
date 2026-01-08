#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "LiquidGravity_Dev/Actors/Enemies/BaseEnemy.h"
#include "SetAnimationState.generated.h"


UCLASS()
class LIQUIDGRAVITY_DEV_API USetAnimationState : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
public:
	USetAnimationState();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
private:
	UPROPERTY(EditAnywhere)
	UAnimationState StateToSet = UAnimationState::Moving;
};
