// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseTask.h"
#include "StuntTask.generated.h"

enum class UStuntEnum : uint8;
class ALiquidCamera;
class ALiquidPlayer;
class ULiquidMovementComponent;
/**
 * 
 */
UCLASS(meta = (DisplayName = "STT_Stunt"))
class LIQUIDGRAVITY_DEV_API UStuntTask : public UBaseTask
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
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Parameter, meta = (AllowPrivateAccess = "true"))
	UDataTable* EnergyAdditionTable;

	float TimeSinceStuntStarted = 0.0f;

	float TimeToStunt = 0.f;
	UStuntEnum CurrentStunt;

	bool TimeToResetFailedStuntStarted = false;
	bool StuntFailed = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Parameter, meta = (AllowPrivateAccess = "true"))
	float TimeToResetFailedStunt = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Parameter, meta = (AllowPrivateAccess = "true"))
	float TimeToSmoothRotate = 8.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Parameter, meta = (AllowPrivateAccess = "true"))
	float RotatePrecision = 0.01f;
};
