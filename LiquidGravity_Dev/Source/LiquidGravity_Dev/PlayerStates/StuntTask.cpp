// Fill out your copyright notice in the Description page of Project Settings.


#include "StuntTask.h"

#include "Kismet/KismetMathLibrary.h"
#include "LiquidGravity_Dev/Components/EnergyComponent.h"
#include "LiquidGravity_Dev/Components/PlayerComponents/LiquidMovementComponent.h"
#include "LiquidGravity_Dev/Components/PlayerComponents/PlayerFeedbackComponent.h"
#include "LiquidGravity_Dev/Components/PlayerComponents/PlayerStuntComponent.h"
#include "LiquidGravity_Dev/Core/LiquidCamera.h"
#include "LiquidGravity_Dev/DataTables/EnergyAdditionDataTable.h"

EStateTreeRunStatus UStuntTask::Tick(FStateTreeExecutionContext& Context, const float DeltaTime)
{
	Super::Tick(Context, DeltaTime);

	TimeSinceStuntStarted += DeltaTime;

	if (TimeSinceStuntStarted > TimeToStunt && !StuntFailed)
	{
		LiquidPlayer->StuntComponent->RemoveStuntTag();
		FStuntStruct stuntStruct;
		stuntStruct.ScoreAddition = 500.f;
		switch (CurrentStunt)
		{
		case UStuntEnum::ForwardBackward:
			stuntStruct.StuntName = "Why are sundays so depressing?";
			stuntStruct.ScoreAddition = 300;
			break;
		case UStuntEnum::BackwardForward:
			stuntStruct.StuntName = "Heaven's Door";
			stuntStruct.ScoreAddition = 250;
			break;
		case UStuntEnum::LeftRight:
			stuntStruct.StuntName = "Black Parade";
			stuntStruct.ScoreAddition = 500;
			break;
		case UStuntEnum::RightLeft:
			stuntStruct.StuntName = "No Surprises";
			stuntStruct.ScoreAddition = 200;
			break;
		}
		LiquidPlayer->StuntComponent->AddStuntStruct(stuntStruct);
		return EStateTreeRunStatus::Succeeded;
	}

	if (!LiquidPlayer->StuntComponent->IsStuntSafe())
	{
		MovementComponent->StartTaskTick();
		MovementComponent->SetDesiredVelocity(FVector::Zero());
		
		MovementComponent->RotateToLookDirection(DeltaTime, TimeToSmoothRotate/4, RotatePrecision);

		//FString stuntString = "Stunt Failed";
		//GEngine->AddOnScreenDebugMessage(-1252,5, FColor::Red, *stuntString);

		MovementComponent->ApplyTaskTick(DeltaTime,1.f,true,MovementComponent->GetAcceleration()/2);

		if (TimeToResetFailedStuntStarted == false)
		{
			TimeSinceStuntStarted = 0.0f;
			TimeToResetFailedStuntStarted = true;
			StuntFailed = true;
			LiquidPlayer->StuntComponent->FailStunt();
		}
		else
		{
			if (TimeSinceStuntStarted > TimeToResetFailedStunt)
			{
				LiquidPlayer->StuntComponent->RemoveStuntTag();
				return EStateTreeRunStatus::Succeeded;
			}
		}
		
		return EStateTreeRunStatus::Running;
	}
	
	const FEnergyAdditionDataTable* energyAddition = EnergyAdditionTable->FindRow<FEnergyAdditionDataTable>(FName("Skate"),FString());

	/*if (TimeSinceStuntStarted > energyAddition->TimeInterval && MovementComponent->GetVelocity().Length() >=
		MovementComponent->GetMaxSpeed()/2 && MovementComponent->IsGrounded())
	{
		LiquidPlayer->EnergyComponent->GainEnergy(energyAddition->EnergyAddition);
		TimeSinceStuntStarted -= energyAddition->TimeInterval;
	}*/
	
	
	MovementComponent->StartTaskTick();
	
	const FRotator camRotation = LiquidCamera->GetActorRotation();

	FVector2D movementVector = MovementComponent->GetInputMovementVector();

	if(movementVector.Length() > 0.5f)
	{
		MovementComponent->LookDirection = UKismetMathLibrary::GetRightVector(camRotation) * MovementComponent->GetInputMovementVector().X + UKismetMathLibrary::GetForwardVector(camRotation) * MovementComponent->GetInputMovementVector().Y;
	}

	MovementComponent->RotateToLookDirection(DeltaTime, TimeToSmoothRotate, RotatePrecision);
	
	MovementComponent->SetDesiredVelocity(FVector{movementVector.Y, movementVector.X, 0.f} * MovementComponent->GetMaxSpeed(),false);
	
	MovementComponent->ApplyTaskTick(DeltaTime,0.9f,true,MovementComponent->GetAcceleration()/2);
	
	return EStateTreeRunStatus::Running;
}

EStateTreeRunStatus UStuntTask::EnterState(FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition)
{
	MovementComponent->SetVelocityWorldSpace(false);

	CurrentStunt = LiquidPlayer->StuntComponent->GetCurrentStunt();
	TimeToStunt = LiquidPlayer->StuntComponent->GetStuntDuration(CurrentStunt);
	TimeSinceStuntStarted = 0.0f;

	LiquidPlayer->FeedbackComponent->OnStuntStarted.Broadcast(CurrentStunt);
	
	return Super::EnterState(Context, Transition);
}

void UStuntTask::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
	Super::ExitState(Context, Transition);
}