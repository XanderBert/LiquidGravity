#include "HookTask.h"

#include "LiquidGravity_Dev/Components/EnergyComponent.h"
#include "LiquidGravity_Dev/Components/PlayerComponents/LiquidMovementComponent.h"
#include "LiquidGravity_Dev/Components/PlayerComponents/PlayerFeedbackComponent.h"
#include "LiquidGravity_Dev/Components/PlayerComponents/PlayerHookingComponent.h"
#include "LiquidGravity_Dev/Core/LiquidPlayer.h"
#include "LiquidGravity_Dev/DataTables/EnergyAdditionDataTable.h"


EStateTreeRunStatus UHookTask::Tick(FStateTreeExecutionContext& Context, const float DeltaTime)
{
	Super::Tick(Context, DeltaTime);

	TimeSinceEnergyAddition += DeltaTime;
	const FEnergyAdditionDataTable* energyAddition = EnergyAdditionDataTable->FindRow<FEnergyAdditionDataTable>(FName("Hook"),FString());
	if (TimeSinceEnergyAddition > energyAddition->TimeInterval)
	{
		LiquidPlayer->EnergyComponent->GainEnergy(energyAddition->EnergyAddition);
		TimeSinceEnergyAddition -= energyAddition->EnergyAddition;
	}
	
	MovementComponent->SetSnapToGround(false);
	MovementComponent->StartTaskTick();

	if(!IsValid(HookableActor))
	{
		return EStateTreeRunStatus::Succeeded;
	}
	
	//Make Desired Velocity towards the Hook
	const FVector direction = ((HookableActor)->GetActorLocation() - LiquidPlayer->GetActorLocation()).GetSafeNormal();
	MovementComponent->SetDesiredVelocity(direction * MaxHookSpeed, true);
	MovementComponent->ApplyTaskTick(DeltaTime,0, true, MaxHookAcceleration);


	const bool bIsAtHookableObject = FVector::Dist(HookableActor->GetActorLocation(),LiquidPlayer->GetActorLocation()) < 200.f;
	const bool bWantsToJump = LiquidPlayer->LiquidMovementComponent->HasJumpTag();
	const bool bWantsToDodge = LiquidPlayer->LiquidMovementComponent->IsDodging();
	
	if(bWantsToJump || bWantsToDodge || bIsAtHookableObject)
	{
		//Move the actor a bit forward so it "jumps" off the grind
		LiquidPlayer->SetActorLocation(LiquidPlayer->GetActorLocation() + LiquidPlayer->GetActorForwardVector()  * 50.0f);
		
		HookingComponent->RemoveHookTag();
		MovementComponent->SetSnapToGround(true);
		HookingComponent->OnHookEnded.Broadcast();

		return EStateTreeRunStatus::Succeeded;
	}
	
	return EStateTreeRunStatus::Running;
}

EStateTreeRunStatus UHookTask::EnterState(FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition)
{
	check(LiquidPlayer)
	check(MovementComponent)
	
	HookingComponent = LiquidPlayer->HookingComponent;
	check(HookingComponent)
	
	HookableActor = HookingComponent->GetTargetHook();


	LiquidPlayer->FeedbackComponent->OnHook.Broadcast();

	return Super::EnterState(Context, Transition);
}