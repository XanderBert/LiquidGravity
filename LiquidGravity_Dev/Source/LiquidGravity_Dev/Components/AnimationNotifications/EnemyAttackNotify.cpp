// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAttackNotify.h"

#include "../EnemyShootingComponent.h"
#include "LiquidGravity_Dev/Actors/Enemies/BaseEnemy.h"

void UEnemyAttackNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	ABaseEnemy* enemy = Cast<ABaseEnemy>(MeshComp->GetOwner());

	if (IsValid(enemy))
	{
		for (auto socket : AttackSockets)
		{
			if (HeavyAttack)
			{
				enemy->EnemyShootingComponent->HeavyShoot(socket);
			}
			else
			{
				enemy->EnemyShootingComponent->Shoot(socket);
			}
		}
	}
}
