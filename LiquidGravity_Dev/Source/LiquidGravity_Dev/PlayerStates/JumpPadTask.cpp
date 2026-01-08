#include "JumpPadTask.h"

#include "LiquidGravity_Dev/Components/BouncePadComponent.h"
#include "LiquidGravity_Dev/Components/EnergyComponent.h"
#include "LiquidGravity_Dev/Components/PlayerComponents/LiquidMovementComponent.h"
#include "LiquidGravity_Dev/Components/PlayerComponents/PlayerFeedbackComponent.h"
#include "LiquidGravity_Dev/Core/LiquidPlayer.h"
#include "LiquidGravity_Dev/DataTables/EnergyAdditionDataTable.h"

EStateTreeRunStatus UJumpPadTask::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
	check(LiquidPlayer)
	check(MovementComponent)
		
	
	BouncePad = MovementComponent->GetBouncePad();
	check(BouncePad)

	
	const FEnergyAdditionDataTable* energyAddition = EnergyAdditionDataTable->FindRow<FEnergyAdditionDataTable>(FName("BouncePad"),FString());
	LiquidPlayer->EnergyComponent->GainEnergy(energyAddition->EnergyAddition);


	LiquidPlayer->FeedbackComponent->OnJumpPad.Broadcast();
	
	return Super::EnterState(Context, Transition);
}


EStateTreeRunStatus UJumpPadTask::Tick(FStateTreeExecutionContext& Context, const float DeltaTime)
{
	Super::Tick(Context, DeltaTime);

	MovementComponent->StartTaskTick();
		

	const float lerpedImpulse = BouncePad->GetImpulseStrength();
	FVector bouncePadUpVelocity = BouncePad->GetImpulseDirection().GetSafeNormal() * lerpedImpulse;
	
	MovementComponent->AddTaskVelocityChange(bouncePadUpVelocity, true);
	MovementComponent->ApplyTaskTick(DeltaTime);
	
	LiquidPlayer->LiquidMovementComponent->RemoveBouncePadTag();
	
	return EStateTreeRunStatus::Succeeded;
}