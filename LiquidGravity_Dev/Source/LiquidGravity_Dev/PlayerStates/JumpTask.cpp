#include "JumpTask.h"

#include "LiquidGravity_Dev/Components/EnergyComponent.h"
#include "LiquidGravity_Dev/Components/PlayerComponents/LiquidMovementComponent.h"
#include "LiquidGravity_Dev/Components/PlayerComponents/PlayerFeedbackComponent.h"
#include "LiquidGravity_Dev/Core/LiquidPlayer.h"
#include "LiquidGravity_Dev/DataTables/EnergyConsumptionDataTable.h"


void UJumpTask::ResetPerformedJump() const
{
	LiquidPlayer->LiquidMovementComponent->RemoveJumpTag();
}

EStateTreeRunStatus UJumpTask::Tick(FStateTreeExecutionContext& Context, const float DeltaTime)
{
	Super::Tick(Context, DeltaTime);
	
	MovementComponent->StartTaskTick();
	MovementComponent->ResetTicksSinceLastJump();
	
	FVector jumpDirection;


	const float airJumpCost = EnergyCostTable->FindRow<FEnergyConsumptionRow>(FName("MidAirJump"),FString())->EnergyConsumption;

	
	if (MovementComponent->GetJumpingFromSpline())
	{
		jumpDirection = LiquidPlayer->GetActorUpVector();
		MovementComponent->SetJumpingFromSpline(false);
	}
	//If we are grounded, jump in the direction is just the contact normal
	else if (MovementComponent->IsGrounded())
	{
		jumpDirection = MovementComponent->GetContactNormal();
		
		//When we can jump, we broadcast the jump event
		LiquidPlayer->FeedbackComponent->OnJump.Broadcast();
	}
	//If we are on a wall
	else if (MovementComponent->IsOnWall())
	{
		jumpDirection = MovementComponent->GetWallNormal()*2;
		
		//When we can jump, we broadcast the jump event
		LiquidPlayer->FeedbackComponent->OnJump.Broadcast();
	}
	//If we are on a steep contact, jump in the direction of the steep normal and reset the jump phase
	else if (MovementComponent->OnSteepContact())
	{
		jumpDirection = MovementComponent->GetSteepNormal();
		
		//When we can jump, we broadcast the jump event
		LiquidPlayer->FeedbackComponent->OnAirJump.Broadcast();

	}
	//If we are in the air and have air jumps left, jump in the direction of the contact normal
	else if (MovementComponent->GetMaxAirJumps() > 0 && LiquidPlayer->EnergyComponent->HasEnergy(airJumpCost))
	{
		jumpDirection = MovementComponent->GetContactNormal();
		LiquidPlayer->FeedbackComponent->OnAirJump.Broadcast();
 		LiquidPlayer->EnergyComponent->LoseEnergy(airJumpCost);
	}
	else
	{
		return EStateTreeRunStatus::Succeeded;
	}


	
	//Reset the ticks since last jump
	MovementComponent->ResetTicksSinceLastJump();
	
	//Calculate the jump speed
	float jumpSpeed = FMath::Sqrt(2.0f * FMath::Abs(MovementComponent->GetGravityAtPlayerPosition().Size()) * MovementComponent->GetJumpHeight());

	//Align the jump direction with the current up axis
	jumpDirection = (jumpDirection + MovementComponent->GetCurrentUpAxis()).GetSafeNormal();
	
	//Add the jump speed to the current velocity
	MovementComponent->AddTaskVelocityChange(jumpDirection * jumpSpeed);
	MovementComponent->ApplyTaskTick(DeltaTime);
	
	return EStateTreeRunStatus::Succeeded;
}

EStateTreeRunStatus UJumpTask::EnterState(FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition)
{

	check(LiquidPlayer)
	check(MovementComponent)

	
	return Super::EnterState(Context, Transition);
}

void UJumpTask::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
	ResetPerformedJump();
	Super::ExitState(Context, Transition);
}
