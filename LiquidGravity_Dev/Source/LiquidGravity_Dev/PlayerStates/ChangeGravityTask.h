// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseTask.h"
#include "ChangeGravityTask.generated.h"

class ALiquidCamera;
class ALiquidPlayer;
class ULiquidMovementComponent;
class UChangeGravityComponent;
/**
 * 
 */
UCLASS(meta = (DisplayName = "STT_ChangeGravity"))
class LIQUIDGRAVITY_DEV_API UChangeGravityTask : public UBaseTask
{
	GENERATED_BODY()

	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) override;
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) override;
	virtual void ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Parameter, meta = (AllowPrivateAccess = "true"))
	ULiquidMovementComponent* MovementComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Parameter, meta = (AllowPrivateAccess = "true"))
	ALiquidPlayer* LiquidPlayer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Parameter, meta = (AllowPrivateAccess = "true"))
	ALiquidCamera* LiquidCamera;
	
	UPROPERTY(EditAnywhere, Category = Parameter)
	UDataTable* EnergyAdditionDataTable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Parameter, meta = (AllowPrivateAccess = "true"))
	float MaxChangeGravitySpeed = 2000.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Parameter, meta = (AllowPrivateAccess = "true"))
	float MaxChangeGravityAcceleration = 20.f;
	
	UPROPERTY()
	UChangeGravityComponent* ChangeGravityComponent;
	
	FVector TargetPosition {} ;
	float TimeSinceEnergyAddition {};};
