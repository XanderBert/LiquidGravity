#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EnergyComponent.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEnergyGainedSignature, float, amount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEnergyLostSignature, float, amount);


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LIQUIDGRAVITY_DEV_API UEnergyComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UEnergyComponent();
	virtual void BeginPlay() override;

	float GetMaxEnergy() const
	{
		return MaxEnergy;
	}


	UPROPERTY(BlueprintAssignable)
	FOnEnergyGainedSignature OnEnergyGained;

	UPROPERTY(BlueprintAssignable)
	FOnEnergyLostSignature OnEnergyLost;
	
	UFUNCTION(BlueprintCallable)
	void LoseEnergy(float amount);

	UFUNCTION(BlueprintCallable)
	void GainEnergy(float amount);

	UFUNCTION(BlueprintCallable)
	bool HasEnergy(float requiredAmount) const{ return CurrentEnergy >= requiredAmount; }
	
	UFUNCTION(BlueprintCallable)
	float GetCurrentEnergy() const { return CurrentEnergy; }

	UFUNCTION(BlueprintCallable)
	float GetEnergyPercentage() const { return CurrentEnergy / MaxEnergy; }
	
private:
	UPROPERTY(EditAnywhere)
	float MaxEnergy{ 100.f };

	UPROPERTY(EditAnywhere)
	float CurrentEnergy{};
};
