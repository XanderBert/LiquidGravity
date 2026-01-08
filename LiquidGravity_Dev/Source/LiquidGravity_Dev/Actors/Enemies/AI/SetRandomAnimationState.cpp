// Fill out your copyright notice in the Description page of Project Settings.


#include "SetRandomAnimationState.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "LiquidGravity_Dev/Actors/Enemies/BaseEnemy.h"

USetRandomAnimationState::USetRandomAnimationState()
{
	bCreateNodeInstance = true;
}

EBTNodeResult::Type USetRandomAnimationState::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);
	//Get SelfActor
	ABaseEnemy* SelfActor = Cast<ABaseEnemy>(OwnerComp.GetBlackboardComponent()->GetValueAsObject("SelfActor"));
	if (!SelfActor)
	{
		return EBTNodeResult::Failed;
	}
	
	//Get a random state from the 
    if (SelfActor->RandomAnimationTimer > 5.f)
    {
    	const int32 randomIndex = FMath::RandRange(0, PossibleStates.Num() - 1);
    	SelfActor->CurrentAnimationState = PossibleStates[randomIndex];
    	SelfActor->RandomAnimationTimer = 0.f;
    }
	
	return EBTNodeResult::Succeeded;
}