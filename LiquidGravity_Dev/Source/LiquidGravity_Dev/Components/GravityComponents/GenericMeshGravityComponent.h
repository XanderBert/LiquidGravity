#pragma once

#include "CoreMinimal.h"
#include "LiquidGravity_Dev/Interfaces/GravityInterface.h"
#include "GenericMeshGravityComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LIQUIDGRAVITY_DEV_API UGenericMeshGravityComponent : public UStaticMeshComponent,  public IGravityInterface
{
	GENERATED_BODY()

public:
	UGenericMeshGravityComponent();
	virtual FVector GetGravity(const FVector& position) override;
	virtual void SetGravity(const FVector& newGravity) override;

private:

	UPROPERTY(EditAnywhere, Category = "Gravity")
	bool InvertRaycast = false;
	UPROPERTY(EditAnywhere, Category = "Gravity")
	bool InvertGravity = false;
	UPROPERTY(EditAnywhere, Category = "Gravity")
	float GravityVelocity = -981.0f;
	UPROPERTY(EditAnywhere, Category = "Gravity")
	bool RaycastToClosestPoint = false;
	UPROPERTY(EditAnywhere, Category = "Gravity")
	float RaycastOffset = 100.f;
};
