// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemySoundNotify.h"

#include "LiquidGravity_Dev/Actors/Enemies/BaseEnemy.h"

void UEnemySoundNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                               const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	ABaseEnemy* enemy = Cast<ABaseEnemy>(MeshComp->GetOwner());
	int randomIndex = FMath::RandRange(0,Sound.Num()-1);
	if (IsValid(enemy))
	{
		enemy->PlaySound(Sound[randomIndex], Volume, PitchRange.X, PitchRange.Y);
	}
}
