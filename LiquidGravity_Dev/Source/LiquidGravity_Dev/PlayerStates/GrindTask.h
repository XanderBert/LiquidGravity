// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseTask.h"
#include "GrindTask.generated.h"

class UGrindSplineComponent;
class ALiquidCamera;
class ALiquidPlayer;
class ULiquidMovementComponent;
/**
 * 
 */
UCLASS(meta = (DisplayName = "STT_Grind"))
class LIQUIDGRAVITY_DEV_API UGrindTask : public UBaseTask
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

	UPROPERTY(EditAnywhere, Category = Parameter)
	float RotationSpeed = 250.f;
	
	UPROPERTY()
	UGrindSplineComponent* SplineToGrindOn;

	UPROPERTY(EditAnywhere, Category = Parameter)
	float GrindAcceleration = 100.f;
	
	UPROPERTY(EditAnywhere, Category = Parameter)
	float MinSpeed = 200.f;

	UPROPERTY(EditAnywhere, Category = Parameter)
	UForceFeedbackEffect* OnGrindStayForceFeedback;
	
	void ResetTimeSinceLastGrind() const;
	
	//Grinding specific stuff
	float TargetSpeed = 0;
	float TraveledDistance = 0;
	bool bEnteredForward = false;
	float AngleRotated = 0;
	float SplineDistance;
	bool bJustEntered = true;

	float TimeSinceEnergyAddition{};
	float Speed{};
};
