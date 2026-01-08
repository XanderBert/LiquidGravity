// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseTask.h"
#include "DodgeTask.generated.h"

class ULiquidMovementComponent;
class ALiquidPlayer;
class ALiquidCamera;
/**
 * 
 */
UCLASS(meta = (DisplayName = "STT_Dodge"))
class LIQUIDGRAVITY_DEV_API UDodgeTask : public UBaseTask
{
	GENERATED_BODY()

	void ResetPerformedDodge() const;
	void ResetTimeSinceLastDodge() const;

	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) override;
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) override;
	
	FVector Direction = FVector(1,1,1);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Parameter, meta = (AllowPrivateAccess = "true"))
	ULiquidMovementComponent* MovementComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Parameter, meta = (AllowPrivateAccess = "true"))
	ALiquidPlayer* LiquidPlayer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Parameter, meta = (AllowPrivateAccess = "true"))
	ALiquidCamera* LiquidCamera;
	
	UPROPERTY(EditAnywhere, Category = Parameter)
	UDataTable* EnergyCostTable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Parameter, meta = (AllowPrivateAccess = "true"))
	float DodgeSpeed;
};
