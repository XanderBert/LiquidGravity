#include "WallRideTask.h"

#include "LiquidMath.h"
#include "Components/CapsuleComponent.h"
#include "LiquidGravity_Dev/Components/EnergyComponent.h"
#include "LiquidGravity_Dev/Components/PlayerComponents/CameraHandler.h"
#include "LiquidGravity_Dev/Components/PlayerComponents/LiquidMovementComponent.h"
#include "LiquidGravity_Dev/Core/LiquidCamera.h"
#include "LiquidGravity_Dev/Core/LiquidPlayer.h"
#include "LiquidGravity_Dev/DataTables/EnergyAdditionDataTable.h"

EStateTreeRunStatus UWallRideTask::Tick(FStateTreeExecutionContext& Context, const float DeltaTime)
{
	Super::Tick(Context, DeltaTime);

	TimeSinceEnergyAddition += DeltaTime;
	const FEnergyAdditionDataTable* energyAddition = EnergyAdditionDataTable->FindRow<FEnergyAdditionDataTable>(FName("WallRide"),FString());
	if (TimeSinceEnergyAddition > energyAddition->TimeInterval)
	{
		LiquidPlayer->EnergyComponent->GainEnergy(energyAddition->EnergyAddition);
		TimeSinceEnergyAddition -= energyAddition->TimeInterval;
	}

	MovementComponent->StartTaskTick();
	
	FVector playerMovementDirection;
	FTransform cameraSpace = LiquidPlayer->CameraHandler->LiquidCamera->GetTransform();
	const FVector cameraSpaceRight = cameraSpace.GetUnitAxis(EAxis::Y);
	const FVector cameraSpaceForward = cameraSpace.GetUnitAxis(EAxis::X);

	MovementComponent->SetCurrentRightAxis(FLiquidMath::ProjectOnContactPlane(cameraSpaceRight, MovementComponent->GetCurrentUpAxis()).GetSafeNormal());
	MovementComponent->SetCurrentForwardAxis(FLiquidMath::ProjectOnContactPlane(cameraSpaceForward, MovementComponent->GetCurrentUpAxis()).GetSafeNormal());

	if (MovementComponent->GetInputMovementVector().Length() > 0.1f)
	{
		FVector2D movementHolder = MovementComponent->GetInputMovementVector();
		playerMovementDirection = movementHolder.X *
		FLiquidMath::ProjectOnContactPlane(cameraSpaceRight, MovementComponent->GetCurrentUpAxis()).GetSafeNormal()
		+ movementHolder.Y *
		FLiquidMath::ProjectOnContactPlane(cameraSpaceForward, MovementComponent->GetCurrentUpAxis()).GetSafeNormal();
	}
	else
	{
		playerMovementDirection = LiquidPlayer->CapsuleComponent->GetPhysicsLinearVelocity();
	}
	
	FVector wallRideDirection = FVector::CrossProduct(MovementComponent->GetWallNormal(),MovementComponent->GetCurrentUpAxis()).GetSafeNormal();
	
	if (FVector::DotProduct(wallRideDirection, playerMovementDirection) < 0)
	{
		wallRideDirection *= -1;
	}

	wallRideDirection -= MovementComponent->GetWallNormal().GetSafeNormal()/2;
	
	const FVector targetVelocity = wallRideDirection * MovementComponent->GetInputMovementVector().Length() * MovementComponent->GetMaxSpeed();
	
	MovementComponent->SetDesiredVelocity(targetVelocity,true);
	MovementComponent->ApplyTaskTick(DeltaTime,0.3f);

	if (!MovementComponent->IsOnWall() || (!MovementComponent->IsGrounded()
		&& MovementComponent->GetVelocity().Length() < MovementComponent->GetMaxSpeed()/3.f))
	{
		LiquidPlayer->LiquidMovementComponent->RemoveWallRidingTag();
		return EStateTreeRunStatus::Succeeded;
	}
	
	const bool bWantsToJump = LiquidPlayer->LiquidMovementComponent->HasJumpTag();
	const bool bWantsToDodge = LiquidPlayer->LiquidMovementComponent->IsDodging();
	
	MovementComponent->ApplyTaskTick(DeltaTime,0);

	if(bWantsToJump || bWantsToDodge)
	{
		LiquidPlayer->LiquidMovementComponent->RemoveWallRidingTag();
		
		//Move the actor a bit forward so it "jumps" off the grind
		LiquidPlayer->SetActorLocation(LiquidPlayer->GetActorLocation() + LiquidPlayer->GetActorForwardVector()  * 50.0f);
		MovementComponent->DisableGrindingTag();
		return EStateTreeRunStatus::Succeeded;
	}

	return EStateTreeRunStatus::Running;
}

void UWallRideTask::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
	Super::ExitState(Context, Transition);
	MovementComponent->RemoveWallRidingTag();
}
