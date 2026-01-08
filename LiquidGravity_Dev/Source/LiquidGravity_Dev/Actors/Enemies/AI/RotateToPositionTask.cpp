

#include "RotateToPositionTask.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "Kismet/KismetMathLibrary.h"

URotateToPositionTask::URotateToPositionTask()
{
	NodeName = TEXT("Rotate To Position");
	bNotifyTick = true;
	bCreateNodeInstance = true;
}

EBTNodeResult::Type URotateToPositionTask::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);
	
	Bot = Cast<APawn>(OwnerComp.GetAIOwner()->GetPawn());
	if(!Bot) return EBTNodeResult::Failed;
	
	FVector currentLocation = Bot->GetActorLocation();
	FVector location = OwnerComp.GetBlackboardComponent()->GetValue<UBlackboardKeyType_Vector>(BlackboardKey.GetSelectedKeyID());
	location.Z = currentLocation.Z;

	//DrawDebugLine(GetWorld(), currentLocation, location, FColor::Red, true, 5.0f, 0, 5.0f);

	StartRotation	= Bot->GetActorRotation();
	TargetRotation	= UKismetMathLibrary::FindLookAtRotation(currentLocation, location);
	
	
	return EBTNodeResult::InProgress;
}

void URotateToPositionTask::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	//FLiquidMath::FastLerpSmooth(StartRotation, TargetRotation, DeltaSeconds, SlerpSpeed, 0.01f).Rotator()
	//Lerp Rotation
	FRotator lerpRot = FMath::RInterpTo(Bot->GetActorRotation(), TargetRotation, DeltaSeconds, SlerpSpeed);
	Bot->SetActorRotation(lerpRot);
	
	if(Bot->GetActorRotation().Equals(TargetRotation, Tolerance))
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}
