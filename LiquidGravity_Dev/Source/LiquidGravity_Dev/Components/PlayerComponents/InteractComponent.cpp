#include "InteractComponent.h"

#include "CameraHandler.h"
#include "LiquidGravity_Dev/Core/LiquidCamera.h"
#include "LiquidGravity_Dev/Core/LiquidPlayer.h"
#include "LiquidGravity_Dev/Interfaces/InteractInterface.h"

UInteractComponent::UInteractComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


void UInteractComponent::BeginPlay()
{
	Super::BeginPlay();
	LiquidPlayer = Cast<ALiquidPlayer>(GetOwner());
}

void UInteractComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	ALiquidCamera* LiquidCamera = LiquidPlayer->CameraHandler->LiquidCamera;
	FuzzySelectStruct fuzzySelectStruct = FuzzySelectStruct();
	fuzzySelectStruct.isInterface = true;
	fuzzySelectStruct.SelectionType = FuzzySelectStruct::Class;
	fuzzySelectStruct.SubClass = UInteractInterface::StaticClass();
	
	const FHitResult* hit = LiquidCamera->GetFuzzySelection(fuzzySelectStruct);
	
	if (hit != nullptr)
	{
		if(HighlightedActor != hit->GetActor())
		{
			if(HighlightedActor) IInteractInterface::Execute_UnHighlight(HighlightedActor);
		}

		if (hit->GetActor() == nullptr) return;
		if (hit->GetActor()->GetClass()->ImplementsInterface(UInteractInterface::StaticClass()))
		{
			IInteractInterface::Execute_Highlight(hit->GetActor());
			HighlightedActor = hit->GetActor();
		}
	}
}

void UInteractComponent::Interact() const
{
	if(HighlightedActor)
	{
		IInteractInterface::Execute_Interact(HighlightedActor);
		IInteractInterface::Execute_UnHighlight(HighlightedActor);
	}
}

