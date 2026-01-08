#include "SkateTask.h"

#include "LiquidMath.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "LiquidGravity_Dev/Components/EnergyComponent.h"
#include "LiquidGravity_Dev/Components/PlayerComponents/LiquidMovementComponent.h"
#include "LiquidGravity_Dev/Components/PlayerComponents/PlayerFeedbackComponent.h"
#include "LiquidGravity_Dev/Components/PlayerComponents/PlayerStuntComponent.h"
#include "LiquidGravity_Dev/Core/LiquidCamera.h"
#include "LiquidGravity_Dev/Core/LiquidPlayer.h"
#include "LiquidGravity_Dev/DataTables/EnergyAdditionDataTable.h"


void USkateTask::OnLandEffects() const
{
	//Velocity on Z Axis
	FVector xAxis, yAxis, zAxis;
	MovementComponent->GetPlayerInputSpaceAxes(xAxis,yAxis,zAxis);

	float zAxisStrength = FMath::Abs(previousFrameVelocity.Dot(MovementComponent->GetGravityAtPlayerPosition().GetSafeNormal()));
	
	LiquidPlayer->FeedbackComponent->OnLand.Broadcast(zAxisStrength);


	//TODO Move to feedback Component
	if (OnLandForceFeedback)
	{
		//Play force feedback
		//UGameplayStatics::GetPlayerController(GetWorld(), 0)->ClientPlayForceFeedback(OnLandForceFeedback);
		LiquidPlayer->FeedbackComponent->PlayForceFeedback(OnLandForceFeedback);
	}

	//TODO Move to feedback Component
	if(OnLandCameraShake.Get())
	{
		//Play camera shake
		//UGameplayStatics::GetPlayerController(GetWorld(), 0)->ClientStartCameraShake(OnLandCameraShake);
		LiquidPlayer->FeedbackComponent->PlayScreenShake(OnLandCameraShake);
	}
}

EStateTreeRunStatus USkateTask::Tick(FStateTreeExecutionContext& Context, const float DeltaTime)
{
	Super::Tick(Context, DeltaTime);
	
	 const FEnergyAdditionDataTable* energyAddition = EnergyAdditionTable->FindRow<FEnergyAdditionDataTable>(FName("Skate"),FString());
	 if (MovementComponent->GetVelocity().Length() >=
	 	MovementComponent->GetMaxSpeed()/2 && MovementComponent->IsGrounded())
	 {
	 	TimeSinceEnergyAddition += DeltaTime;

	 	if (TimeSinceEnergyAddition > energyAddition->TimeInterval)
	 	{
	 		LiquidPlayer->EnergyComponent->GainEnergy(energyAddition->EnergyAddition);
	 		TimeSinceEnergyAddition -= energyAddition->TimeInterval;
	 	}
	 }
	
	MovementComponent->StartTaskTick();

	if (isGroundedTracker == false && MovementComponent->IsGrounded())
	{
		OnLandEffects();
	}
	
	isGroundedTracker = MovementComponent->IsGrounded();
	previousFrameVelocity = MovementComponent->GetVelocity();
	
	const FRotator camRotation = LiquidCamera->GetActorRotation();

	FVector2D movementVector = MovementComponent->GetInputMovementVector();

	if(movementVector.Length() > 0.5f)
	{
		MovementComponent->LookDirection = UKismetMathLibrary::GetRightVector(camRotation) * MovementComponent->GetInputMovementVector().X + UKismetMathLibrary::GetForwardVector(camRotation) * MovementComponent->GetInputMovementVector().Y;
	}

	MovementComponent->RotateToLookDirection(DeltaTime, TimeToSmoothRotate, RotatePrecision);


	float baseMultiplier = 0.9f;
	baseMultiplier += LiquidPlayer->StuntComponent->GetCurrentMultiplier() * 0.125f;

	MovementComponent->SetDesiredVelocity(FVector{movementVector.Y, movementVector.X, 0.f} * MovementComponent->GetMaxSpeed() * baseMultiplier,false);

	if (movementVector.Length() < 0.1f && LiquidPlayer->GetVelocity().Length() < 100.f && MovementComponent->IsGrounded())
	{
		MovementComponent->SetTaskVelocity(FLiquidMath::LerpSmooth(MovementComponent->GetVelocity(), FVector::ZeroVector, DeltaTime, 0.5f, 0.001f), DeltaTime, true);
		MovementComponent->ApplyTaskTick(DeltaTime,0.2f);
	}
	else
	{
		MovementComponent->ApplyTaskTick(DeltaTime,1,true,MovementComponent->GetAcceleration() * baseMultiplier);
	}
	
	return EStateTreeRunStatus::Succeeded;
}

EStateTreeRunStatus USkateTask::EnterState(FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition)
{

	check(LiquidPlayer)
	check(MovementComponent)
	
	LiquidPlayer->FeedbackComponent->OnSkateStarted.Broadcast();
	MovementComponent->SetVelocityWorldSpace(false);

	return Super::EnterState(Context, Transition);
}