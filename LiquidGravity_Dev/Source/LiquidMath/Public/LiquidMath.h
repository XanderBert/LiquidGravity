#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

 
class LIQUIDMATH_API FLiquidMath
{
public:
    [[nodiscard]] static float LerpSmooth(float a, float b, float dt, float h);
    [[nodiscard]] static float LerpSmooth(float a, float b, float dt, float t, float precision);
    [[nodiscard]] static FVector LerpSmooth(const FVector& a, const FVector& b, float dt, float h);
    [[nodiscard]] static FVector LerpSmooth(const FVector& a, const FVector& b, float dt, float t, float precision);
    [[nodiscard]] static FQuat FastLerpSmooth(const FQuat& a, const FQuat& b, float dt, float h);
    [[nodiscard]] static FQuat FastLerpSmooth(const FQuat& a, const FQuat& b, float dt, float t, float precision);
    [[nodiscard]] static FQuat SlerpSmooth(const FQuat& a, const FQuat& b, float dt, float h);
    [[nodiscard]] static FQuat SlerpSmooth(const FQuat& a, const FQuat& b, float dt, float t, float precision);
    [[nodiscard]] static FVector ProjectOnContactPlane(const FVector& direction, const FVector& normal);
    [[nodiscard]] static float MoveTowards(float current, float target, float maxDelta);

private:
    static float GetHalfLife(float t, float precision);
};

class FLiquidMathModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
};


