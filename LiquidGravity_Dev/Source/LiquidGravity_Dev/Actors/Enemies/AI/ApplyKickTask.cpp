// Fill out your copyright notice in the Description page of Project Settings.


#include "ApplyKickTask.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "LiquidGravity_Dev/Actors/Enemies/BaseEnemy.h"
#include "LiquidGravity_Dev/Core/LiquidPlayer.h"

UApplyKickTask::UApplyKickTask()
{
	bNotifyTick = true;
	bCreateNodeInstance = true;
}

EBTNodeResult::Type UApplyKickTask::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Bot = Cast<ABaseEnemy>(OwnerComp.GetBlackboardComponent()->GetValueAsObject("SelfActor"));
	if(Bot)
	{
		//If is being kicked
		if(Bot->GetIsKick())
		{
			ALiquidPlayer* alp = Cast<ALiquidPlayer>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));

			KickDirection = alp->GetActorForwardVector();
			UCapsuleComponent* capsule = Bot->GetComponentByClass<UCapsuleComponent>();

			capsule->AddImpulse(KickDirection * 250.f * Bot->GetKickMultiplier() + UKismetMathLibrary::RandomUnitVector() * 50.f
				* Bot->GetKickMultiplier(), NAME_None, true);
			capsule->AddTorqueInRadians(Bot->GetKickMultiplier() * 25.f * KickDirection
				+ UKismetMathLibrary::RandomUnitVector() * 25.f
				* Bot->GetKickMultiplier(), NAME_None, true);
			ElapsedTime = 0.0f;
			return EBTNodeResult::InProgress;
		}
	}
	
	return EBTNodeResult::Failed;
}

void UApplyKickTask::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	ElapsedTime += DeltaSeconds;

	ALiquidPlayer* alp = Cast<ALiquidPlayer>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	if (IsValid(alp))
	{
	}
	if(PlaceBotOnGround)
	{
		float distanceToGround = 0.0f;
		FHitResult hitResult;
		FCollisionQueryParams collisionParams;
		collisionParams.AddIgnoredActor(Bot);
		if(GetWorld()->LineTraceSingleByChannel(hitResult, Bot->GetActorLocation(), Bot->GetActorLocation() - FVector(0.0f, 0.0f, 1000.0f), ECollisionChannel::ECC_WorldStatic, collisionParams))
		{
			distanceToGround = hitResult.Distance;
		}
		
		//Apply Subtle force to the bot to make it go down
		const FVector downForce = FVector(0.0f, 0.0f, (-distanceToGround * 50.0f) + 750.0f) * 100 * DeltaSeconds;
		Bot->CapsuleComponent->AddForce(downForce, NAME_None, true);
	}

	
	if(ElapsedTime >= TimeUntilDisable)
	{
		Bot->DisableKick();
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}

