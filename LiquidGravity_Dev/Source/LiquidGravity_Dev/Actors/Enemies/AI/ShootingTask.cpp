#include "ShootingTask.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "LiquidGravity_Dev/Actors/Enemies/BaseEnemy.h"
#include "LiquidGravity_Dev/Components/EnemyShootingComponent.h"

UShootingTask::UShootingTask()
{
	bCreateNodeInstance = true;
}

EBTNodeResult::Type UShootingTask::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	//Get the owner bot
	const ABaseEnemy* bot = Cast<ABaseEnemy>(OwnerComp.GetBlackboardComponent()->GetValueAsObject("SelfActor"));
	if(bot)
	{
		bot->EnemyShootingComponent->bIsAttacking = true;
	}
	
	return EBTNodeResult::Succeeded;
}
