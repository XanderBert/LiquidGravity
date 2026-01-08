// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/BoxComponent.h"
#include "ContextualGravitySwitchComponent.generated.h"


class UGenericMeshGravityComponent;
class UGravityInterface;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LIQUIDGRAVITY_DEV_API UContextualGravitySwitchComponent : public UBoxComponent
{
	GENERATED_BODY()

public:
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	// Sets default values for this component's properties
	UContextualGravitySwitchComponent();

	UPROPERTY(EditAnywhere)
	AActor* PreviousGravityInterfaceToSwitch {};
	UPROPERTY(EditAnywhere)
	AActor* MyGravityInterface{};

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:
};
