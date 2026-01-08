#include "GoToNextLocationTask.h"

#include "LiquidMath.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "Kismet/KismetMathLibrary.h"
#include "LiquidGravity_Dev/Actors/Enemies/BaseEnemy.h"


UGoToNextLocationTask::UGoToNextLocationTask()
{
	NodeName = TEXT("Go To Next Location");
	bNotifyTick = true;
	bCreateNodeInstance = true;
}

EBTNodeResult::Type UGoToNextLocationTask::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	//Get the owner bot
	Bot = Cast<ABaseEnemy>(OwnerComp.GetBlackboardComponent()->GetValueAsObject("SelfActor"));
	FVector start = Bot->GetActorLocation();
	NextLocation = OwnerComp.GetBlackboardComponent()->GetValue<UBlackboardKeyType_Vector>("NextLocation");

	float distance = FVector::Dist(start, NextLocation);

	Speed = (distance / 100.0f) * TimeToMoveAMeter;

	
	return EBTNodeResult::InProgress;
}

void UGoToNextLocationTask::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
	
	FVector start = Bot->GetActorLocation();

	 	
	if(FVector::Dist(start, NextLocation) < AcceptableRange)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	} 
	
	FVector newLocation = FLiquidMath::LerpSmooth(start, NextLocation, DeltaSeconds,  Speed, 0.1f);


	//Rotate the bot to face the target
	FRotator currentRotation = Bot->GetActorRotation();
	FRotator newRotation = UKismetMathLibrary::FindLookAtRotation(Bot->GetActorLocation(), newLocation);

	//Clamp Pitch Rotation
	newRotation.Pitch = FMath::Clamp(newRotation.Pitch, ClampedPitch.X, ClampedPitch.Y);
	
	
	FVector smoothRotation = FLiquidMath::LerpSmooth(currentRotation.Vector(), newRotation.Vector(), DeltaSeconds, TimeToRotate, 0.1f);

	Bot->SetActorRotation(smoothRotation.Rotation(), ETeleportType::None);

	
	//Move the bot
	if(!Bot->SetActorLocation(newLocation, true))
	{
		//Get a random location in a radius of 300
		FVector randomLocation = UKismetMathLibrary::RandomPointInBoundingBox(Bot->GetActorLocation(), FVector(500, 500, 500));
		NextLocation = randomLocation;
	}
}