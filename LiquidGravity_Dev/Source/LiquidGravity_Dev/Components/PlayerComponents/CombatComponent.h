#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "NiagaraSystem.h"
#include "CombatComponent.generated.h"


class ALiquidPlayer;
class AThrowableActor;
class ALiquidCamera;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LIQUIDGRAVITY_DEV_API UCombatComponent : public UWidgetComponent
{
	GENERATED_BODY()

public:
	UCombatComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	void EvaluateCombat();
	void EndEvaluateCombat();
	

	void Pickup(AThrowableActor* Actor);
	bool HasPickedUpActor() const;

	UFUNCTION(BlueprintPure)
	bool GetIsLockingOn () const
	{
		return bIsLockingOn;
	}
	void SetIsLockingOn (bool value)
	{
		bIsLockingOn = value;
	}

	void LockOn();
	AActor* GetLockedOnActor () const
	{
		return LockedOnActor;
	}
	UFUNCTION(BlueprintPure)
	AThrowableActor* GetPickedUpActor () const
	{
		return PickedUpActor;
	}

private:
	//-----------Shooting--------------
	FTimerHandle ShootTimerHandle;
	void StartShooting();
	void StopShooting();
	void Shoot();
	UPROPERTY(EditAnywhere, Category = "Shooting")
	float ShootInterval = 0.1f;
	
	UPROPERTY(EditAnywhere, Category="Shooting")
	float ShootDistance = 1000.0f;

	UPROPERTY(EditAnywhere, Category="Shooting")
	UNiagaraSystem* ShootingFX;


	//-----------Kick--------------
	FTimerHandle KickTimerHandle;

	
	
	bool IsEnemyInCloseRange() const;
	void StartKick();
	void StopKick();
	void Kick();
	
	UPROPERTY(EditAnywhere, Category="Kick")
	float KickInterval = 1.f;
	
	UPROPERTY(EditAnywhere, Category="Kick")
	float KickDistance = 100.0f;

	//----------Throw---------
	UPROPERTY()
	AThrowableActor* PickedUpActor{};

	UPROPERTY(EditAnywhere, Category="Throw")
	float ThrowSpeed = 3000.0f;
	

	void Throw();




	FVector GetTarget() const;
	
	
	void EnableWidget(const FVector& impactPoint);
	void DisableWidget();

	
	UPROPERTY(EditAnywhere, Category="Combat")
	float ShootDamage = 15.0f;

	UPROPERTY(EditAnywhere, Category="Combat")
	float KickDamage = 35.0f;
	
	UPROPERTY(EditAnywhere, Category="Combat")
	float KickForce = 2000.0f;
	
	bool bIsTargetting = false;
	FVector TargettingLocation{};
	UPROPERTY()
	AActor* TargettingActor{};
	
	bool bIsLockingOn = false;
	
	UPROPERTY(EditAnywhere, Category = Parameter)
	UDataTable* EnergyCostTable;

	UPROPERTY()
	ALiquidPlayer* LiquidPlayer;

	UPROPERTY()
	AActor* LockedOnActor;
};
