#pragma once
#include "CoreMinimal.h"

#include "CustomGravity.generated.h"
class IGravityInterface;



USTRUCT()
struct FCustomGravity
{
	GENERATED_BODY()
	
public:
	FVector GetGravity(const FVector& position) const;
	
	void GetGravity(const FVector& position, FVector& gravity, FVector& upAxis) const;
	FVector GetUpAxis(const FVector& position) const;

	FQuat GetGravityAlignment(const FVector& position, const FQuat& CurrentAlignment) const;
	
	void SetActiveGravity(IGravityInterface* BaseField);
	void SetGravitySource(IGravityInterface* BaseField, AActor* gravityFieldActor);
	void SetIsForcedLockedGravity(bool forced, const FVector& Gravity);

	IGravityInterface* GetGravitySource() const { return CurrentGravitySource; }
	AActor* GetGravityFieldActor() const { return GravityFieldActor; }

	void OnGameEnd();
	bool GetIsForcedGravity() const { return IsForcedLockedGravity; };

private:

	bool IsForcedLockedGravity = false;
	FVector ForcedLockedGravityValue = FVector::Zero();
	
	UPROPERTY(EditAnywhere, Category = "Gravity")
	float GravityVelocity = -981.0f;

	UPROPERTY(EditAnywhere, Category = "Gravity")
	float GravityMultiplier = 3.0f;

	
	FVector ActiveGravity = FVector(0, 0, GravityVelocity);
	
	IGravityInterface* CurrentGravitySource{};

	UPROPERTY()
	AActor* GravityFieldActor{};
};

