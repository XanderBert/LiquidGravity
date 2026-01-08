#include "MoveAndRotateTo.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/KismetMathLibrary.h"

UMoveAndRotateTo::UMoveAndRotateTo()
{
	NodeName = TEXT("NavMesh Move and Rotate To");
	bNotifyTick = true;
	bCreateNodeInstance = true;
}

EBTNodeResult::Type UMoveAndRotateTo::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);
	
	//Get the Current Selected Key from this action
	auto selectedKey = GetSelectedBlackboardKey();
	Target = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(selectedKey));
	Bot = Cast<APawn>(OwnerComp.GetBlackboardComponent()->GetValueAsObject("SelfActor"));

	if(!Target)
		return EBTNodeResult::Failed;

	if(!Bot)
		return EBTNodeResult::Failed;

	FVector targetLocation = Target->GetActorLocation();
	targetLocation.Z = Bot->GetActorLocation().Z;
	
	//Move to Target using NavMesh
	Cast<AAIController>(Bot->GetController())->MoveToLocation(targetLocation, 5.0f, true, true, false);
	
	return  EBTNodeResult::InProgress;	
}

void UMoveAndRotateTo::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
	
	//Current Rotation
	auto currentRotation = Bot->GetActorRotation();

	//Target Rotation
	auto targetRotation = UKismetMathLibrary::FindLookAtRotation(Bot->GetActorLocation(), Target->GetActorLocation());

	//Clamp [-10, 10] the Target Rotation Up and Down
	targetRotation.Pitch = FMath::Clamp(targetRotation.Pitch, -10.0f, 10.0f);

	
	//Slerp Rotation
	auto newRotation = FMath::RInterpTo(currentRotation, targetRotation, GetWorld()->DeltaTimeSeconds, 3.0f);
	
	Bot->SetActorRotation(newRotation);


	if(currentRotation.Equals(targetRotation, 15.0f))
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}
