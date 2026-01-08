#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "HealthComponent.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerDiedSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerHealedSignature,int, amount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnPlayerDamagedSignature,int, amount, AActor*, damagingActor, FVector, damageLocation);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LIQUIDGRAVITY_DEV_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UHealthComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	void TakeDamage(int DamageAmount, AActor* damagingActor, FVector damageLocation);

	
	UFUNCTION(BlueprintCallable)
	void Heal(float HealAmount);

	UFUNCTION(BlueprintCallable)
	float GetHealthPercentage() const { return  static_cast<float>(CurrentHealth) / static_cast<float>(MaxHealth); }
	
	UFUNCTION(BlueprintGetter, Blueprintable)
	int GetCurrentHealth() const { return CurrentHealth; }

	int GetMaxHealth() const { return MaxHealth; };


	UPROPERTY(BlueprintAssignable)
	FOnPlayerDiedSignature OnPlayerDied{};

	UPROPERTY(BlueprintAssignable)
	FOnPlayerHealedSignature OnPlayerHealed{};

	UPROPERTY(BlueprintAssignable)
	FOnPlayerDamagedSignature OnPlayerDamaged{};

	UPROPERTY(EditAnywhere)
	bool HasRegeneratingHealth = false;
	
private:
	UPROPERTY(EditAnywhere, Category = "Health")
	int MaxHealth{100};
	float CurrentHealth{};
	float TimeSinceRegen = 0.f;
	float TimeToStartRegen = 7.f;
	float RegenRate = 5.f;
	bool OnPlayerDiedBroadcasted = false;
};