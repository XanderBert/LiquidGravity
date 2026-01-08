// Fill out your copyright notice in the Description page of Project Settings.


#include "CompositeGravityComponent.h"


// Sets default values for this component's properties
UCompositeGravityComponent::UCompositeGravityComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UCompositeGravityComponent::BeginPlay()
{
	Super::BeginPlay();
	IsComposite = false;
	TArray<USceneComponent*> children;
	GetChildrenComponents(true, children);
	for (int i = 0; i < children.Num(); i++)
	{
		if(children[i]->Implements<UGravityInterface>())
		{
			GravityInterfaces.Add(Cast<IGravityInterface>(children[i]));
			GravityInterfaces[GravityInterfaces.Num()-1]->IsComposite=true;
			GravityInterfaces[GravityInterfaces.Num()-1]->CompositeGravityInterface=this;
		}
	}

	// ...
	
}


// Called every frame
void UCompositeGravityComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                               FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UCompositeGravityComponent::SetGravity(const FVector& newGravity)
{
	//no op
}

FVector UCompositeGravityComponent::GetGravity(const FVector& position)
{
	FVector GravityNormal = FVector::Zero();

	bool flag {false};

	ClosestGravityInterface = GravityInterfaces[0];
	float closestGravityLength = UE_BIG_NUMBER;

	for (int i = 0; i < GravityInterfaces.Num(); i++)
	{
		USceneComponent* us = Cast<USceneComponent>(GravityInterfaces[i]->_getUObject());
		if (us == nullptr)
		{
			continue;
		}
		
		float distance = (position - us->GetComponentLocation()).Length();

		UMeshComponent *um = Cast<UMeshComponent>(us);
		
		if (IsValid(um))
		{
			FVector endpoint{};
			um->GetClosestPointOnCollision(position,endpoint);
			distance = (position - endpoint).Length();
		}
		
		if (distance < MaxDistance)
		{
			GravityNormal += GravityInterfaces[i]->GetGravity(position);
			flag = true;
		}
		if (distance < closestGravityLength)
		{
			ClosestGravityInterface = GravityInterfaces[i];
			closestGravityLength = distance;
		}
	}
	GravityNormal.Normalize();
	
	if (flag)
	{
		return -GravityNormal * GravityStrength;;
	}
	
	return ClosestGravityInterface->GetGravity(position);
}

