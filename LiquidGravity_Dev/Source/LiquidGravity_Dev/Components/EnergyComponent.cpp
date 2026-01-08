#include "EnergyComponent.h"


UEnergyComponent::UEnergyComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UEnergyComponent::BeginPlay()
{
	Super::BeginPlay();
	CurrentEnergy = MaxEnergy;
}

void UEnergyComponent::LoseEnergy(float amount)
{
	CurrentEnergy -= amount;
	OnEnergyLost.Broadcast(amount);
}

void UEnergyComponent::GainEnergy(float amount)
{
	CurrentEnergy += amount;
	CurrentEnergy = FMath::Min(CurrentEnergy, MaxEnergy);
	OnEnergyGained.Broadcast(amount);
}