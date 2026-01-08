#include "ChasePlayerTask.h"

#include "EnemyAiController.h"
#include "LiquidMath.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "LiquidGravity_Dev/Actors/Enemies/BaseEnemy.h"
#include "LiquidGravity_Dev/Actors/Enemies/EnemyBullet.h"
#include "LiquidGravity_Dev/Core/LiquidPlayer.h"

UChasePlayerTask::UChasePlayerTask()
{
	bNotifyTick = true;
	bCreateNodeInstance = true;
}

EBTNodeResult::Type UChasePlayerTask::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	//Get the player character
	const FName playerKey = GetSelectedBlackboardKey();
	Player = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(playerKey));

	if(!Player)
	{
		ResetBlackboard();
		UE_LOG(LogTemp, Error, TEXT("No Player found"));
		return EBTNodeResult::Failed;
	} 
	
	AiController = Cast<AEnemyAiController>(OwnerComp.GetAIOwner());
	if (!AiController)
	{
		ResetBlackboard();
		UE_LOG(LogTemp, Error, TEXT("No AI Controller found"));
		return EBTNodeResult::Failed;
	} 
	
	Bot = AiController->GetPawn();
	if(!Bot)
	{
		ResetBlackboard();
		UE_LOG(LogTemp, Error, TEXT("No Bot found"));
		return EBTNodeResult::Failed;
	} 
	

	FVector location = Bot->GetActorLocation();
	FVector forward = (Player->GetActorLocation() - location).GetSafeNormal();
	
	FVector playerUp = Player->GetActorUpVector();
	FVector playerLocation = Player->GetActorLocation();

	FVector chaseLocation = location + forward * ChaseDistance;
	
	//Check what the closest distance is
	float distanceToPlayer = FVector::Dist(location, playerLocation);
	float distanceToTarget = FVector::Dist(location, chaseLocation);

	//Set the target to the closest distance
	Target = distanceToPlayer < distanceToTarget ? playerLocation : chaseLocation;
	
	//Make it so that the target is above the player
	Target = playerUp * HeightToHoverAbovePlayer + Target;

	
	FCollisionQueryParams collisionParams;
	collisionParams.AddIgnoredActor(Bot);
	collisionParams.AddIgnoredActor(Player);

	

	//Reset the time with no line of sight
	BlackboardComponent = OwnerComp.GetBlackboardComponent();
	BlackboardComponent->SetValueAsFloat("SecondsWithNoLOF", 0.0f);
	
	return EBTNodeResult::InProgress;
}

void UChasePlayerTask::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
	
	const FVector start = Bot->GetActorLocation();
	const FVector playerLocation = Player->GetActorLocation();

	
	const bool bIsCloseToTarget = FVector::Dist(start, Target) <= 10.0f;
	const bool bIsCloseToPlayer = FVector::Dist(start, playerLocation) <= DistanceFromPlayer;
	if(bIsCloseToTarget || bIsCloseToPlayer)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		return;
	}

	
	
	//Check if the player is still accessible
	FHitResult Hit;
	FCollisionQueryParams collisionParams;
	collisionParams.AddIgnoredActor(Bot);
	collisionParams.AddIgnoredActor(Player);
	
	if(GetWorld()->LineTraceSingleByChannel(Hit, start, playerLocation, ECollisionChannel::ECC_Visibility, collisionParams))
	{
		//If the hit is not the player or another enemy we lost line of sight
		if(!Hit.GetActor()->IsA<ALiquidPlayer>() && !Hit.GetActor()->IsA<ABaseEnemy>())
		{
			
			float passedNoLOF = BlackboardComponent->GetValueAsFloat("SecondsWithNoLOF");
			passedNoLOF += DeltaSeconds;

			if(passedNoLOF >= TimeWithNoLineOfSightUnitlItLosesInterest)
			{
				ResetBlackboard();
				FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
			}
			else
			{
				//GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Red, FString::Printf(TEXT("Seconds with no LOF: %f"), passedNoLOF));
				BlackboardComponent->SetValueAsFloat("SecondsWithNoLOF", passedNoLOF);
			}
		}	
	}else
	{
		BlackboardComponent->SetValueAsFloat("SecondsWithNoLOF", 0.0f);
	}
	
	const FVector newLocation = FLiquidMath::LerpSmooth(start, Target, DeltaSeconds, TimeToMoveToTarget, 0.5f);
	const FRotator newRotation = UKismetMathLibrary::FindLookAtRotation(Bot->GetActorLocation(), Player->GetActorLocation());
	const FVector smoothRotation = FLiquidMath::LerpSmooth(Bot->GetActorRotation().Vector(), newRotation.Vector(), DeltaSeconds, TimeToRotate, 0.1f);
	
	Bot->SetActorLocationAndRotation(newLocation, smoothRotation.Rotation(), true);
}

void UChasePlayerTask::ResetBlackboard()
{
	//Reset the Player key to initial state
	BlackboardComponent->SetValueAsObject("Player", nullptr);
	AiController->SetPlayerDiscovered(false);
	AiController->SetPlayerDiscoveredAnimPlayed(false);
	Cast<ABaseEnemy>(Bot)->OnPlayerLost.Broadcast();
}
