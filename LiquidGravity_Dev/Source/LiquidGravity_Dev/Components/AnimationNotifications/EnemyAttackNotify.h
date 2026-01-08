// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "EnemyAttackNotify.generated.h"

/**
 * 
 */
UCLASS()
class LIQUIDGRAVITY_DEV_API UEnemyAttackNotify : public UAnimNotify
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TArray<FName> AttackSockets = TArray<FName>{
		FName("ShootingSocket")
	};

	UPROPERTY(EditAnywhere)
	bool HeavyAttack = false;

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
};
