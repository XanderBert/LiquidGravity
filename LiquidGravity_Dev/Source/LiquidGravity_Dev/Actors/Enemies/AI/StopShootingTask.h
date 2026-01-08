// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "StopShootingTask.generated.h"

/**
 * 
 */
UCLASS()
class LIQUIDGRAVITY_DEV_API UStopShootingTask : public UBTTask_BlackboardBase
{
	UStopShootingTask();
	
	GENERATED_BODY()
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
