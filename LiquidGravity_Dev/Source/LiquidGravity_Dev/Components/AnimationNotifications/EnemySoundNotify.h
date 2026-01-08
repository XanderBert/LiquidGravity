// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "EnemySoundNotify.generated.h"

/**
 * 
 */
UCLASS()
class LIQUIDGRAVITY_DEV_API UEnemySoundNotify : public UAnimNotify
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TArray<USoundBase*> Sound {};
	UPROPERTY(EditAnywhere)
	float Volume = 1.f;
	UPROPERTY(EditAnywhere)
	FVector2D PitchRange = FVector2D(0.8f, 1.2f);
	
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
};
