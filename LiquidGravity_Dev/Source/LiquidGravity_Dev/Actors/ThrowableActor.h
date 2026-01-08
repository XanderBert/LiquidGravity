#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ThrowableActor.generated.h"


class UCapsuleComponent;

UCLASS()
class LIQUIDGRAVITY_DEV_API AThrowableActor : public AActor
{
	GENERATED_BODY()

public:
	AThrowableActor();

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = Components)
	UStaticMeshComponent* StaticMeshComponent;


	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components)
	UCapsuleComponent* CapsuleComponent;

	
	void SetThrown(const FVector& Force);
	void SetPull() const;
	bool GetIsThrown() const
	{
		return bIsThrown;
	}
	void SetIsPickedUp(bool value)
	{
		bIsPickedUp = value;
	}
	UFUNCTION(BlueprintPure)
	bool GetIsPickedUp() const
	{
		return bIsPickedUp;
	}

	UPROPERTY(EditAnywhere)
	FVector2D LeftUpDelta {};


private:

	//-------------------- Components  --------------------

	
	UPROPERTY(EditAnywhere)
	float TimeToResetThrow = 1.5f;

	UPROPERTY(EditAnywhere)
	float DamageOnThrow = 20.f;

	
	UFUNCTION()
	void OnCapsuleOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	void ResetThrow();
	FTimerHandle ThrowTimerHandle;

	bool bIsThrown{ false };
	bool bIsPickedUp { false };
};
