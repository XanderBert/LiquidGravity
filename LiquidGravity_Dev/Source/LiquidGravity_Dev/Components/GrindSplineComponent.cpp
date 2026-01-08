#include "GrindSplineComponent.h"

#include "PlayerComponents/LiquidMovementComponent.h"
#include "Components/SphereComponent.h"
#include "LiquidGravity_Dev/Core/LiquidPlayer.h"

UGrindSplineComponent::UGrindSplineComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}



void UGrindSplineComponent::BeginPlay()
{
	Super::BeginPlay();
	//GetOwner()->OnActorBeginOverlap.AddDynamic(this, &UGrindSplineComponent::OnOverlap);
}

void UGrindSplineComponent::TickComponent(float DeltaTime, enum ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	AActor* owner = GetOwner();
	TArray<AActor*> overlappingActors;
	owner->GetOverlappingActors(overlappingActors);
	for(auto overlapActor: overlappingActors)
	{
		OnOverlap(overlapActor);
	}
}

FVector UGrindSplineComponent::GetNextGrindLocation(const FVector& CurrentLocation, const FVector& forward, const FVector upAxis, const float Distance) const
{
	const FVector pointToSearchFor = CurrentLocation + forward * Distance;
	const float splineInputKey = FindInputKeyClosestToWorldLocation(pointToSearchFor);
	FVector nextSplinePoint = GetLocationAtSplineInputKey(splineInputKey, ESplineCoordinateSpace::World);

	nextSplinePoint += PlayerUpDisplacement * upAxis;
	return nextSplinePoint;
	
}

FVector UGrindSplineComponent::GetNextGrindLocationTask(float TraveledDistance, const FVector upAxis, const float Distance) const
{
	FVector nextSplinePoint = GetLocationAtDistanceAlongSpline(TraveledDistance + Distance, ESplineCoordinateSpace::World);
	nextSplinePoint += PlayerUpDisplacement * upAxis;
	return nextSplinePoint;
	
}

FVector UGrindSplineComponent::GetSplinePointAtStartEnd(bool Start, const FVector& upAxis) const
{
	FVector nextSplinePoint = GetLocationAtTime(Start ? 0 : Duration, ESplineCoordinateSpace::World);

	nextSplinePoint += PlayerUpDisplacement * upAxis;
	return nextSplinePoint;
}

FVector UGrindSplineComponent::GetUpAxisAtDistance(const float Distance) const
{
	const FVector up = GetUpVectorAtDistanceAlongSpline(Distance, ESplineCoordinateSpace::World);
	return up;

	
	/*const auto pointTransform = GetTransformAtDistanceAlongSpline(Distance, ESplineCoordinateSpace::World,true);
	const FVector upVector = pointTransform.GetUnitAxis(EAxis::Z);
	return upVector;*/
}

void UGrindSplineComponent::OnOverlap(AActor* OtherActor)
{
	if (OtherActor->IsA<ALiquidPlayer>())
	{
		ALiquidPlayer* liquidPlayer = Cast<ALiquidPlayer>(OtherActor);
		if (liquidPlayer->FootOverlappingSphere->IsOverlappingActor((GetOwner())) &&
			!liquidPlayer->LiquidMovementComponent->HasChangeGravityTag())
		{
			liquidPlayer->LiquidMovementComponent->TryGrindRail(this);
		}
	}
}
