#include "StopShootingTask.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "LiquidGravity_Dev/Actors/Enemies/BaseEnemy.h"
#include "LiquidGravity_Dev/Components/EnemyShootingComponent.h"

UStopShootingTask::UStopShootingTask()
{
	bCreateNodeInstance = true;
}

EBTNodeResult::Type UStopShootingTask::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);


	//Get the owner bot
	const ABaseEnemy* bot = Cast<ABaseEnemy>(OwnerComp.GetBlackboardComponent()->GetValueAsObject("SelfActor"));

	if (!bot)
	{
		return EBTNodeResult::Failed;
	}
	
	bot->EnemyShootingComponent->bIsAttacking = false;


	return EBTNodeResult::Succeeded;
}
