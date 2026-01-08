// Fill out your copyright notice in the Description page of Project Settings.


#include "SetAnimationState.h"

#include "BehaviorTree/BlackboardComponent.h"

USetAnimationState::USetAnimationState()
{
	bCreateNodeInstance = true;
}

EBTNodeResult::Type USetAnimationState::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);
	
	//Get SelfActor
	ABaseEnemy* SelfActor = Cast<ABaseEnemy>(OwnerComp.GetBlackboardComponent()->GetValueAsObject("SelfActor"));
	if (!SelfActor)
	{
		return EBTNodeResult::Failed;
	}

	SelfActor->CurrentAnimationState = StateToSet;
	
	return EBTNodeResult::Succeeded;
}
