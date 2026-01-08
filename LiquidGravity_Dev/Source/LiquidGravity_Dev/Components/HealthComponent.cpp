
#include "HealthComponent.h"

#include "LiquidGravity_Dev/Core/LiquidPlayer.h"


UHealthComponent::UHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();
	CurrentHealth = MaxHealth;
	if (GetOwner()->IsA<ALiquidPlayer>())
	{
		CurrentHealth = MaxHealth/2;
	}
	OnPlayerDiedBroadcasted = false;
}

void UHealthComponent::TickComponent(float DeltaTime, enum ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (HasRegeneratingHealth)
	{
		TimeSinceRegen += DeltaTime;
		if (TimeSinceRegen >= TimeToStartRegen)
		{
			if (GetCurrentHealth() < MaxHealth * 2 / 3)
				Heal(RegenRate * DeltaTime);
		}
	}

	CurrentHealth = FMath::Clamp(CurrentHealth, 0, MaxHealth);
}


void UHealthComponent::TakeDamage(int DamageAmount, AActor* damagingActor, FVector damageLocation)
{
	TimeSinceRegen = 0.f;
	CurrentHealth -= DamageAmount;
	OnPlayerDamaged.Broadcast(DamageAmount, damagingActor, damageLocation);
	if(CurrentHealth <= 0)
	{
		CurrentHealth = 0;
		if (!OnPlayerDiedBroadcasted)
		{
			OnPlayerDied.Broadcast();
			OnPlayerDiedBroadcasted = true;
		}
	}
}

void UHealthComponent::Heal(float HealAmount)
{
	CurrentHealth += HealAmount;
	OnPlayerHealed.Broadcast(HealAmount);
	if(CurrentHealth > MaxHealth)
	{
		CurrentHealth = MaxHealth;
	}	
}

