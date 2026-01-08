#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "PickupComponent.generated.h"


class AThrowableActor;
class ALiquidPlayer;
class UCombatComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LIQUIDGRAVITY_DEV_API UPickupComponent : public UWidgetComponent
{
	GENERATED_BODY()

public:
	UPickupComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	
	void Pickup();

private:
	//Picking up and throwing
	float TimeToPull = 2.f;
	float PullTimer = 0.f;
	float PickUpAnimatorTimer = 0.f;

	FVector PickupInitialPosition;
	FRotator PickupInitialRotation;
	
	void EnableWidget(const FVector& impactLocation, AThrowableActor* actor);
	void DisableWidget();


	//-----------Pulling--------------
	void EnablePulling();
	void Pull(float deltaTime);
	void FinishPull();
	
	bool bIsPulling = false;
	FVector PullingLocation{};
	


	//-----------LockingOn--------------
	bool bIsLockingOn = false;
	UPROPERTY()
	AThrowableActor* LockOnTarget{};
	FVector LockingOnLocation{};
	


	UPROPERTY()
	ALiquidPlayer* LiquidPlayer{};

	UPROPERTY()
	UCombatComponent* CombatComponent{};
};
