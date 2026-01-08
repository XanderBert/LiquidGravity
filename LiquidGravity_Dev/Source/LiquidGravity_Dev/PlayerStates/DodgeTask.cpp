#include "DodgeTask.h"

#include "LiquidMath.h"
#include "LiquidGravity_Dev/Components/EnergyComponent.h"
#include "LiquidGravity_Dev/Components/PlayerComponents/LiquidMovementComponent.h"
#include "LiquidGravity_Dev/Components/PlayerComponents/PlayerFeedbackComponent.h"
#include "LiquidGravity_Dev/Core/LiquidPlayer.h"
#include "LiquidGravity_Dev/DataTables/EnergyConsumptionDataTable.h"

void UDodgeTask::ResetPerformedDodge() const
{
	MovementComponent->DisableDodgingTag();
}

void UDodgeTask::ResetTimeSinceLastDodge() const
{
	MovementComponent->TimeSinceLastDodge = 0;
}

EStateTreeRunStatus UDodgeTask::Tick(FStateTreeExecutionContext& Context, const float DeltaTime)
{
	Super::Tick(Context, DeltaTime);

	ResetPerformedDodge();
	ResetTimeSinceLastDodge();
	
	MovementComponent->StartTaskTick();
	MovementComponent->ResetTicksSinceLastJump();
	const float energyCost = EnergyCostTable->FindRow<FEnergyConsumptionRow>(FName("Dodge"),FString())->EnergyConsumption;

	if (LiquidPlayer->EnergyComponent->HasEnergy(energyCost))
	{
		LiquidPlayer->EnergyComponent->LoseEnergy(energyCost);
		LiquidPlayer->FeedbackComponent->OnDodge.Broadcast();
	}
	else
	{
		return EStateTreeRunStatus::Succeeded;
	}
	
	FVector dodgeDirection {};
	
	//If we are grounded, jump in the direction is just the contact normal
	if (MovementComponent->IsGrounded())
	{
		dodgeDirection += MovementComponent->GetContactNormal().GetSafeNormal();
	}
	else
	{
		dodgeDirection += MovementComponent->GetCurrentUpAxis();
	}

	//If we are inputting a direction, dodge in that direction
	if (MovementComponent->GetInputMovementVector().Length() > 0.1f)
	{
		FVector CurrentRightAxis = MovementComponent->GetCurrentRightAxis();
		FVector CurrentForwardAxis = MovementComponent->GetCurrentForwardAxis();

		//Since dodge direction holds up vector right now
		FVector xAxis = FLiquidMath::ProjectOnContactPlane(CurrentRightAxis, dodgeDirection).GetSafeNormal();
		FVector yAxis = FLiquidMath::ProjectOnContactPlane(CurrentForwardAxis, dodgeDirection).GetSafeNormal();

		dodgeDirection += (xAxis * MovementComponent->GetInputMovementVector().X + yAxis * MovementComponent->GetInputMovementVector().Y)*4;
	}
	else
	{
		dodgeDirection += LiquidPlayer->GetActorForwardVector() * 4;
	}
	dodgeDirection.Normalize();
	
	 
	//Add the jump speed to the current velocity
	MovementComponent->SetTaskVelocity(dodgeDirection * DodgeSpeed + MovementComponent->GetVelocity()/2.f);
	MovementComponent->ApplyTaskTick(DeltaTime);
	
	return EStateTreeRunStatus::Succeeded;
}

EStateTreeRunStatus UDodgeTask::EnterState(FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition)
{
	check(LiquidPlayer)
	check(MovementComponent)

	return Super::EnterState(Context, Transition);
}