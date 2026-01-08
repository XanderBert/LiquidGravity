#include "WaypointVisualizer.h"

#include "BouncePadVisualizer.h"
#include "UnrealEdGlobals.h"
#include "Visualizer.h"
#include "Editor/UnrealEdEngine.h"
#include "LiquidGravity_Dev/Components/BouncePadComponent.h"
#include "LiquidGravity_Dev/Components/EnemeyWaypointComponent.h"

#define LOCTEXT_NAMESPACE "FWaypointVisualizerModule"

void FWaypointVisualizerModule::StartupModule()
{
	if (GUnrealEd)
	{
		const TSharedPtr<FWaypointVisualizer> Visualizer = MakeShareable(new FWaypointVisualizer);
		if (Visualizer.IsValid())
		{
			GUnrealEd->RegisterComponentVisualizer(UEnemeyWaypointComponent::StaticClass()->GetFName(), Visualizer);
			Visualizer->OnRegister();
		}
		
		const TSharedPtr<FBouncePadVisualizer> bouncePadVisualizer = MakeShareable(new FBouncePadVisualizer);
		if(bouncePadVisualizer.IsValid())
		{
			GUnrealEd->RegisterComponentVisualizer(UBouncePadComponent::StaticClass()->GetFName(), bouncePadVisualizer);
			bouncePadVisualizer->OnRegister();
		}
	}
}

void FWaypointVisualizerModule::ShutdownModule()
{
	if (GUnrealEd)
	{
		GUnrealEd->UnregisterComponentVisualizer(UEnemeyWaypointComponent::StaticClass()->GetFName());
		GUnrealEd->UnregisterComponentVisualizer(UBouncePadComponent::StaticClass()->GetFName());
	}
}

#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FWaypointVisualizerModule, WaypointVisualizer)