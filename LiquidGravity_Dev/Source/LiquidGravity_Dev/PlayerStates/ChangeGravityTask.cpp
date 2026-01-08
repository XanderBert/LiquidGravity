#include "ChangeGravityTask.h"

#include "LiquidGravity_Dev/Components/PlayerComponents/ChangeGravityComponent.h"
#include "LiquidGravity_Dev/Components/PlayerComponents/LiquidMovementComponent.h"
#include "LiquidGravity_Dev/Components/PlayerComponents/PlayerFeedbackComponent.h"
#include "LiquidGravity_Dev/Components/PlayerComponents/PlayerStuntComponent.h"
#include "LiquidGravity_Dev/Core/LiquidPlayer.h"

EStateTreeRunStatus UChangeGravityTask::Tick(FStateTreeExecutionContext& Context, const float DeltaTime)
{
	Super::Tick(Context, DeltaTime);
	
	TargetPosition = ChangeGravityComponent->GetTargetPosition();

	const bool bIsAtGravityPosition = FVector::Dist(TargetPosition,LiquidPlayer->GetActorLocation()) < 1500.f;
	
	const bool bWantsToJump = LiquidPlayer->LiquidMovementComponent->HasJumpTag();
	const bool bWantsToDodge = LiquidPlayer->LiquidMovementComponent->IsDodging();

	if(bWantsToJump || bWantsToDodge || bIsAtGravityPosition)
	{
		MovementComponent->SetLockGravity(false,true);
		ChangeGravityComponent->RemoveChangeGravityTag();
		MovementComponent->SetSnapToGround(true);

		return EStateTreeRunStatus::Succeeded;
	}
	
	TimeSinceEnergyAddition += DeltaTime;
	
	MovementComponent->SetLockGravity(true,false);
	MovementComponent->SetSnapToGround(false);
	MovementComponent->StartTaskTick();
	
	FVector direction = (TargetPosition - LiquidPlayer->GetActorLocation()).GetSafeNormal();

	MovementComponent->AddTaskVelocityChange((direction * MaxChangeGravitySpeed) * DeltaTime, true);
	MovementComponent->ApplyTaskTick(DeltaTime,0);
	
	return EStateTreeRunStatus::Running;
}

EStateTreeRunStatus UChangeGravityTask::EnterState(FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition)
{
	check(LiquidPlayer)
	check(MovementComponent)
	
	ChangeGravityComponent = LiquidPlayer->ChangeGravityComponent;
	check(ChangeGravityComponent)
	LiquidPlayer->LiquidMovementComponent->SetLockGravity(true,true);
	TargetPosition = ChangeGravityComponent->GetTargetPosition();

	const bool bIsAtGravityPosition = FVector::Dist(TargetPosition,LiquidPlayer->GetActorLocation()) < 1000.f;

	if (!bIsAtGravityPosition)
	{
		LiquidPlayer->FeedbackComponent->OnChangeGravity.Broadcast();
		FStuntStruct stuntStruct;
		stuntStruct.StuntName = "Gravity Change";
		stuntStruct.ScoreAddition = 250;
		LiquidPlayer->StuntComponent->AddStuntStruct(stuntStruct);
	}

	return Super::EnterState(Context, Transition);
}

void UChangeGravityTask::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
	LiquidPlayer->LiquidMovementComponent->SetLockGravity(false,true);
	
	Super::ExitState(Context, Transition);
}