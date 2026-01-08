#include "LiquidMath.h"

#define LOCTEXT_NAMESPACE "FLiquidMathModule"



float FLiquidMath::GetHalfLife(float t, float precision)
{
	return -t / FMath::Log2(precision);
}

float FLiquidMath::LerpSmooth(float a, float b, float dt, float h)
{
	return b + (a-b) * FMath::Pow(2,-dt/h);
}

float FLiquidMath::LerpSmooth(float a, float b, float dt, float t, float precision)
{
	return b + (a - b) * FMath::Pow(2, -dt / GetHalfLife(t, precision));
}

FVector FLiquidMath::LerpSmooth(const FVector& a, const FVector& b, float dt, float h)
{
	return b + (a-b) * FMath::Pow(2,-dt/h);
}

FVector FLiquidMath::LerpSmooth(const FVector& a, const FVector& b, float dt, float t, float precision)
{
	return b + (a - b) * FMath::Pow(2, -dt / GetHalfLife(t, precision));
}

FQuat FLiquidMath::FastLerpSmooth(const FQuat& a, const FQuat& b, float dt, float h)
{
	return (FQuat::FastLerp(a, b, 1 - FMath::Pow(2, -dt / h)));
}

FQuat FLiquidMath::FastLerpSmooth(const FQuat& a, const FQuat& b, float dt, float t, float precision)
{
	return (FQuat::FastLerp(a, b, 1 - FMath::Pow(2, -dt / GetHalfLife(t, precision))));
}

FQuat FLiquidMath::SlerpSmooth(const FQuat& a, const FQuat& b, float dt, float h)
{
	return (FQuat::Slerp(a, b, 1 - FMath::Pow(2, -dt / h)));
}

FQuat FLiquidMath::SlerpSmooth(const FQuat& a, const FQuat& b, float dt, float t, float precision)
{
	return (FQuat::Slerp(a, b, 1 - FMath::Pow(2, -dt / GetHalfLife(t, precision))));
}

FVector FLiquidMath::ProjectOnContactPlane(const FVector& direction, const FVector& normal)
{
	return (direction - normal * direction.Dot(normal)).GetSafeNormal();
}

float FLiquidMath::MoveTowards(float current, float target, float maxDelta)
{
	if (FMath::Abs(target - current) <= maxDelta)
	{
		return target;
	}
	return current + FMath::Sign(target - current) * maxDelta;
}



void FLiquidMathModule::StartupModule()
{
    
}

void FLiquidMathModule::ShutdownModule()
{
    
}




#undef LOCTEXT_NAMESPACE
IMPLEMENT_MODULE(FLiquidMathModule, LiquidMath)


