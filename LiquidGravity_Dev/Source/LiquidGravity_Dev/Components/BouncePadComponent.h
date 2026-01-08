// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/StaticMeshComponent.h"
#include "BouncePadComponent.generated.h"

/**
 * 
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LIQUIDGRAVITY_DEV_API UBouncePadComponent : public UStaticMeshComponent
{
	GENERATED_BODY()

public:
	UBouncePadComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintGetter)
	FVector GetImpulseDirection() const { return ImpulseDirection; }

	UFUNCTION(BlueprintGetter)
	float GetImpulseStrength() const { return ImpulseStrength; }

	UFUNCTION(BlueprintGetter)
	float GetMaxImpulseStrength() const { return MaxImpulseStrength; };

	UFUNCTION(BlueprintSetter)
	void SetImpulseStrength(float NewImpulseStrength) { ImpulseStrength = NewImpulseStrength; }


private:
	UPROPERTY(EditAnywhere)
	float ImpulseStrength{ 350.f };

	UPROPERTY(EditAnywhere)
	float MaxImpulseStrength { 350.f };
	
	UPROPERTY(EditAnywhere)
	FVector ImpulseDirection = FVector::UpVector;
};
