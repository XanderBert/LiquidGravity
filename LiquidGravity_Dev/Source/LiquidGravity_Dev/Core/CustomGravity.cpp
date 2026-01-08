#include "CustomGravity.h"

#include "LiquidGravity_Dev/Interfaces/GravityInterface.h"

void FCustomGravity::SetActiveGravity(IGravityInterface* BaseField)
{
	ActiveGravity = BaseField->GetGravity(FVector::ZeroVector);
}

void FCustomGravity::SetGravitySource(IGravityInterface* BaseField, AActor* gravityFieldActor)
{
	CurrentGravitySource = BaseField;
	GravityFieldActor = gravityFieldActor;
}

void FCustomGravity::SetIsForcedLockedGravity(bool forced, const FVector& Gravity)
{
	IsForcedLockedGravity = forced;
	ForcedLockedGravityValue = Gravity;
}

void FCustomGravity::OnGameEnd()
{
	CurrentGravitySource = nullptr;
}


FVector FCustomGravity::GetGravity(const FVector& position) const
{
	if (IsForcedLockedGravity)
	{
		return ForcedLockedGravityValue;
	}
	
	FVector Gravity = FVector::ZeroVector;

	if(CurrentGravitySource != nullptr)
	{
		Gravity += CurrentGravitySource->GetGravity(position);
	}
	
	//If we are currently not in a 
	if(Gravity.IsNearlyZero())
	{
		Gravity = FVector(0, 0, GravityVelocity);
	}
	return Gravity * GravityMultiplier;
}

void FCustomGravity::GetGravity(const FVector& position, FVector& gravity, FVector& upAxis) const
{
	gravity = GetGravity(position);
	upAxis = -gravity.GetSafeNormal();
}

FVector FCustomGravity::GetUpAxis(const FVector& position) const
{
	return -GetGravity(position).GetSafeNormal();
}

FQuat FCustomGravity::GetGravityAlignment(const FVector& position, const FQuat& CurrentAlignment) const
{
	const FVector from = CurrentAlignment * FVector(0,0,1);
	const FVector to = GetUpAxis(position);
	return FQuat::FindBetweenVectors(from, to) * CurrentAlignment;
}


