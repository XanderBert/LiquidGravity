// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "ApplyKickTask.generated.h"

class ABaseEnemy;
/**
 * 
 */
UCLASS()
class LIQUIDGRAVITY_DEV_API UApplyKickTask : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
public:
	UApplyKickTask();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
private:

	UPROPERTY()
	ABaseEnemy* Bot{};

	FVector KickDirection {};
	
	UPROPERTY(EditAnywhere)
	float TimeUntilDisable{4.0f};
	float ElapsedTime{};

	UPROPERTY(EditAnywhere)
	bool PlaceBotOnGround{false};
};
