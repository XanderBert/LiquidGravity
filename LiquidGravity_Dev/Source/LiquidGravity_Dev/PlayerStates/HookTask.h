// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseTask.h"
#include "HookTask.generated.h"

class ALiquidCamera;
class AHookableActor;
class ALiquidPlayer;
class ULiquidMovementComponent;
class UPlayerHookingComponent;
/**
 * 
 */
UCLASS(meta = (DisplayName = "STT_Hook"))
class LIQUIDGRAVITY_DEV_API UHookTask : public UBaseTask
{
	GENERATED_BODY()

	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) override;
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) override;
	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Parameter, meta = (AllowPrivateAccess = "true"))
	ULiquidMovementComponent* MovementComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Parameter, meta = (AllowPrivateAccess = "true"))
	ALiquidPlayer* LiquidPlayer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Parameter, meta = (AllowPrivateAccess = "true"))
	ALiquidCamera* LiquidCamera;
	
	UPROPERTY(EditAnywhere, Category = Parameter)
	UDataTable* EnergyAdditionDataTable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Parameter, meta = (AllowPrivateAccess = "true"))
	float MaxHookSpeed = 2000.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Parameter, meta = (AllowPrivateAccess = "true"))
	float MaxHookAcceleration = 20.f;
	
	UPROPERTY()
	UPlayerHookingComponent* HookingComponent;
	
	UPROPERTY()
	AHookableActor* HookableActor;

	float TimeSinceEnergyAddition {};
};