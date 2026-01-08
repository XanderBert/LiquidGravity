#pragma once

#include "CoreMinimal.h"
#include "Components/SplineComponent.h"
#include "GrindSplineComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LIQUIDGRAVITY_DEV_API UGrindSplineComponent : public USplineComponent
{
	GENERATED_BODY()

public:
	UGrindSplineComponent();

	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	UFUNCTION(BlueprintGetter)
	bool HasCylindricalRotation() const { return bHasCylindricalRotation; }

	UFUNCTION(BlueprintGetter)
	bool IsEndless() const { return bIsEndless; }

	UFUNCTION(BlueprintCallable)
	FVector GetNextGrindLocation(const FVector& CurrentLocation, const FVector& forward, const FVector upAxis, const float Distance = 15.0f) const;
	FVector GetNextGrindLocationTask(float TraveledDistance, FVector upAxis, float Distance) const;
	FVector GetSplinePointAtStartEnd(bool Start, const FVector& upAxis) const;

	UFUNCTION(BlueprintCallable)
	FVector GetUpAxisAtDistance(const float Distance) const;


private:
	
	UFUNCTION() void OnOverlap(AActor* OtherActor);
	UPROPERTY(EditAnywhere)
	bool bHasCylindricalRotation{};
	UPROPERTY(EditAnywhere)
	float PlayerUpDisplacement = 100.f;
	UPROPERTY(EditAnywhere)
	bool bIsEndless{};
};
