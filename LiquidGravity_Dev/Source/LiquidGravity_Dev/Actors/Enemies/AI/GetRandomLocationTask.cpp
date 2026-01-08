#include "GetRandomLocationTask.h"

#include "EnemyAiController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "LiquidGravity_Dev/Actors/Enemies/BaseEnemy.h"
#include "LiquidGravity_Dev/Actors/Enemies/Waypoint.h"
#include "LiquidGravity_Dev/Components/EnemeyWaypointComponent.h"
#include "LiquidGravity_Dev/Components/WaypointUsageComponent.h"
#include "LiquidGravity_Dev/Core/LiquidPlayer.h"


UGetRandomLocationTask::UGetRandomLocationTask()
{
	bCreateNodeInstance = true;
}

EBTNodeResult::Type UGetRandomLocationTask::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AEnemyAiController* MyController = Cast<AEnemyAiController>(OwnerComp.GetAIOwner());
	
	 if (!MyController)
	 {
	 	UE_LOG(LogTemp, Error, TEXT("GetRandomLocationTask: Failed to get AI Controller"));
	 	return EBTNodeResult::Failed;
	 } 
	
	 if (const APawn* bot = MyController->GetPawn())
	 {

	 	//Get the bot
		const ABaseEnemy* enemy = Cast<ABaseEnemy>(bot);
	 	if(enemy == nullptr)
	 	{
	 		UE_LOG(LogTemp, Error, TEXT("GetRandomLocationTask: Failed to get enemy"));
	 		return EBTNodeResult::Failed;
	 	}

	 	//Get the waypointUsageComponent
	 	UWaypointUsageComponent* waypointUsageComponent = enemy->GetComponentByClass<UWaypointUsageComponent>();
	 	if(waypointUsageComponent == nullptr)
	 	{
	 		auto name = enemy->GetName();
	 		UE_LOG(LogTemp, Error, TEXT("GetRandomLocationTask: Failed to get waypointUsageComponent for %s"), *name);
	 		return EBTNodeResult::Failed;
	 	} 

	 	//Get the waypointComponent
	 	auto& waypoint = waypointUsageComponent->SelectedWaypoint;
		if(waypoint == nullptr)
		{
			auto name = enemy->GetName();
			UE_LOG(LogTemp, Error, TEXT("GetRandomLocationTask: Failed to get waypoint for %s"), *name);
			return EBTNodeResult::Failed;
		} 
	 	
	 	
	 	FVector start = bot->GetActorLocation();
	 	FVector end{};
	 	
	 	FHitResult hitResult;
	 	FCollisionQueryParams collisionParams;
	 	collisionParams.AddIgnoredActor(bot);
	 	
	 	bool waypointFound = false;
	 	TArray<FVector> waypoints = waypointUsageComponent->Waypoints;
		for(int i{}; i < waypoints.Num(); ++i)
		{
			end = waypointUsageComponent->CycleWaypoint(SnapPointToGround);
			
			if(MyController->GetWorld()->LineTraceSingleByChannel(hitResult, start, end, ECC_Visibility, collisionParams))
			{
				//If it hit something that is not a base enemy or a liquid player we try another waypoint
				if(!hitResult.GetActor()->IsA<ABaseEnemy>() || !hitResult.GetActor()->IsA<ALiquidPlayer>())
				{
					continue;
				}
			}
			

			waypointFound = true;
			break;
		}
	 	
	 	if(!waypointFound)
	 	{
	 		auto name = enemy->GetName();
	 		UE_LOG(LogTemp, Error, TEXT("GetRandomLocationTask: Failed to find a waypoint for %s"), *name);
	 		return EBTNodeResult::Failed;
	 	} 
	 	
	 	OwnerComp.GetBlackboardComponent()->SetValue<UBlackboardKeyType_Vector>(BlackboardKey.GetSelectedKeyID(), end);
	 	return EBTNodeResult::Succeeded;
	 }

	UE_LOG(LogTemp, Error, TEXT("GetRandomLocationTask: Failed"));
	return EBTNodeResult::Failed;
}

