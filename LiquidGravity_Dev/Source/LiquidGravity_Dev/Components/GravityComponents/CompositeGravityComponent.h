// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "LiquidGravity_Dev/Interfaces/GravityInterface.h"
#include "CompositeGravityComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LIQUIDGRAVITY_DEV_API UCompositeGravityComponent : public USceneComponent, public IGravityInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UCompositeGravityComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	TArray<IGravityInterface*> GravityInterfaces;
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
							   FActorComponentTickFunction* ThisTickFunction) override;

	virtual void SetGravity(const FVector& newGravity) override;
	virtual FVector GetGravity(const FVector& position) override;

	IGravityInterface* ClosestGravityInterface = nullptr;
	
	UPROPERTY(EditAnywhere)
	float MaxDistance = 1000.f;
	
	UPROPERTY(EditAnywhere)
	float GravityStrength = -981.0f;
};
