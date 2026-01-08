// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseTask.h"
#include "JumpTask.generated.h"

class UEnergyComponent;
class ALiquidCamera;
class ALiquidPlayer;
class ULiquidMovementComponent;
/**
 * 
 */
UCLASS(meta = (DisplayName = "STT_Jump"))
class LIQUIDGRAVITY_DEV_API UJumpTask : public UBaseTask
{
	GENERATED_BODY()

	void ResetPerformedJump() const;
	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) override;
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) override;
	virtual void ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) override;

	FVector Direction = FVector(1,1,1);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Parameter, meta = (AllowPrivateAccess = "true"))
	ULiquidMovementComponent* MovementComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Parameter, meta = (AllowPrivateAccess = "true"))
	ALiquidPlayer* LiquidPlayer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Parameter, meta = (AllowPrivateAccess = "true"))
	ALiquidCamera* LiquidCamera;
	
	UPROPERTY(EditAnywhere, Category = Parameter)
	UDataTable* EnergyCostTable;
};
