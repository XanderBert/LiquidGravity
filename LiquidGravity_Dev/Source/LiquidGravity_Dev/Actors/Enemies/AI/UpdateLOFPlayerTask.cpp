#include "UpdateLOFPlayerTask.h"

#include "EnemyAiController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "LiquidGravity_Dev/Actors/Enemies/BaseEnemy.h"
#include "LiquidGravity_Dev/Actors/Enemies/EnemyBullet.h"
#include "LiquidGravity_Dev/Core/LiquidPlayer.h"

UUpdateLOFPlayerTask::UUpdateLOFPlayerTask()
{
	bCreateNodeInstance = true;
}

EBTNodeResult::Type UUpdateLOFPlayerTask::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);
	
	//Get the blackboard key
	FName playerKey = GetSelectedBlackboardKey();
	AActor* player = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(playerKey));
	ABaseEnemy* bot = Cast<ABaseEnemy>(OwnerComp.GetBlackboardComponent()->GetValueAsObject("SelfActor"));

	
	if(player == nullptr) return EBTNodeResult::Failed;
	if(bot == nullptr) return EBTNodeResult::Failed;
	
	TArray<FHitResult> hitResult{};
	FCollisionQueryParams params;
	params.AddIgnoredActor(bot);
	
	if(OwnerComp.GetWorld()->LineTraceMultiByChannel(hitResult, bot->GetActorLocation(), player->GetActorLocation(), ECollisionChannel::ECC_Visibility))
	{

		for(FHitResult hit : hitResult)
		{
			if(hit.GetActor()->IsA<ALiquidPlayer>() || hit.GetActor()->IsA<ABaseEnemy>() || hit.GetActor()->IsA<AEnemyBullet>())
			{
				return EBTNodeResult::Succeeded;
			}

			auto* hitActor = hit.GetActor();
			//GEngine->AddOnScreenDebugMessage(-1 , 5.f, FColor::Purple, hitActor->GetName());
		}
		
		
		
		//Reset the Player key to initial state
		OwnerComp.GetBlackboardComponent()->ClearValue(playerKey);
		AEnemyAiController* enemyAI = Cast<AEnemyAiController>(bot->Controller);
		enemyAI->SetPlayerDiscovered(false);
		enemyAI->SetPlayerDiscoveredAnimPlayed(false);
		bot->OnPlayerLost.Broadcast();
	}
	
	return EBTNodeResult::Succeeded;
}
