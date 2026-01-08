#include "GenericMeshGravityComponent.h"

#include "Kismet/GameplayStatics.h"


UGenericMeshGravityComponent::UGenericMeshGravityComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


FVector UGenericMeshGravityComponent::GetGravity(const FVector& position)
{
	//TODO: Make it calculate centre if mesh is not centred
	FVector meshCentre = GetComponentLocation();

	if(RaycastToClosestPoint)
	{
		GetClosestPointOnCollision(position,meshCentre);
	}
	
	TArray <FHitResult> hits;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(UGameplayStatics::GetPlayerController(GetWorld(),0)->GetPawn());

	FVector hitNormal = FVector::Zero();
	FVector startPoint;
	FVector endPoint;

	if (InvertRaycast)
	{
		startPoint = meshCentre;
		endPoint = position;
	}
	else
	{
		startPoint = position;
		endPoint = meshCentre;
	}
	
	FVector direction = (endPoint - startPoint).GetSafeNormal();

	if (InvertRaycast)
	{
		startPoint-=direction*RaycastOffset;
	}
	else
	{
		endPoint+=direction*RaycastOffset;
	}
	

	if(GetWorld()->SweepMultiByChannel(hits, startPoint, endPoint, FQuat::Identity,ECC_Visibility,FCollisionShape::MakeSphere(5),Params))
	{
		for (int i{0}; i < hits.Num(); ++i)	
		{
			if(hits[i].GetActor() == this->GetOwner())
			{
				hitNormal += hits[i].Normal;
			}
			else
			{
				hits.RemoveAt(i);
				--i;
			}
		}
	}

	if (hits.Num() <= 0)
	{
		hitNormal = startPoint - endPoint;
	}

	
	hitNormal.Normalize();
	
	if (InvertGravity)
	{
		hitNormal*=-1;
	}
	
	
	return hitNormal * GravityVelocity;
}

void UGenericMeshGravityComponent::SetGravity(const FVector& newGravity)
{
	GravityVelocity = newGravity.Z;
}

