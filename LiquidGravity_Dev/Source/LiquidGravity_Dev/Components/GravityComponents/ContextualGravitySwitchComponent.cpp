// Fill out your copyright notice in the Description page of Project Settings.


#include "ContextualGravitySwitchComponent.h"

#include "CompositeGravityComponent.h"
#include "GenericMeshGravityComponent.h"
#include "LiquidGravity_Dev/Components/PlayerComponents/LiquidMovementComponent.h"
#include "LiquidGravity_Dev/Core/LiquidGameInstance.h"
#include "LiquidGravity_Dev/Core/LiquidPlayer.h"
#include "LiquidGravity_Dev/Interfaces/GravityInterface.h"


void UContextualGravitySwitchComponent::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                                       UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ALiquidPlayer* player = Cast<ALiquidPlayer>(OtherActor);
	ULiquidGameInstance* gameInstance = Cast<ULiquidGameInstance>(GetWorld()->GetGameInstance());
	
	if (IsValid(player) && IsValid(gameInstance))
	{
		
		TArray<UGenericMeshGravityComponent*> myGravityInterfaces;
		TArray<UGenericMeshGravityComponent*> previousGravityInterfaces;

		MyGravityInterface->GetComponents<UGenericMeshGravityComponent*>(myGravityInterfaces,true);
		PreviousGravityInterfaceToSwitch->GetComponents<UGenericMeshGravityComponent*>(previousGravityInterfaces,true);

		bool wasPreviousCorrect = false;

		for (auto interface: previousGravityInterfaces)
		{
			IGravityInterface* gi = Cast<IGravityInterface>(interface);
			IGravityInterface* debugInterface = gameInstance->Gravity.GetGravitySource();
			if (gi != nullptr)
			{
				if (gameInstance->Gravity.GetGravitySource() == gi)
				{
					wasPreviousCorrect = true;
					break;
				}
			}
		}

		TArray<UCompositeGravityComponent*> previousCompositeGravityInterfaces;
		PreviousGravityInterfaceToSwitch->GetComponents<UCompositeGravityComponent>(previousCompositeGravityInterfaces,true);

		for (auto interface: previousCompositeGravityInterfaces)
		{
			IGravityInterface* gi = Cast<IGravityInterface>(interface);
			IGravityInterface* debugInterface = gameInstance->Gravity.GetGravitySource();
			if (gi != nullptr)
			{
				if (gameInstance->Gravity.GetGravitySource() == gi)
				{
					wasPreviousCorrect = true;
					break;
				}
			}
		}

		if (player->LiquidMovementComponent->GetLockGravity() || gameInstance->Gravity.GetIsForcedGravity()
			|| gameInstance->Gravity.GetGravitySource() == nullptr)
		{
			wasPreviousCorrect = true;
		}
	
		if (wasPreviousCorrect)
		{
			for (auto interface: myGravityInterfaces)
			{
				IGravityInterface* gi = Cast<IGravityInterface>(interface);
				if (gi->IsComposite)
				{
					gi->CompositeGravityInterface->Execute_SetGravityFieldActive(true, interface->GetOwner());
					break;
				}
				gi->Execute_SetGravityFieldActive(true, interface->GetOwner());
			}
		}
	}
}

// Sets default values for this component's properties
UContextualGravitySwitchComponent::UContextualGravitySwitchComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
}


// Called when the game starts
void UContextualGravitySwitchComponent::BeginPlay()
{
	Super::BeginPlay();

	OnComponentBeginOverlap.AddDynamic(this, &UContextualGravitySwitchComponent::OnOverlapBegin);
}
