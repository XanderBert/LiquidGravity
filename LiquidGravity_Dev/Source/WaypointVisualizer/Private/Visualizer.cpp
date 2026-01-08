#include "Visualizer.h"

#include "LiquidGravity_Dev/Components/EnemeyWaypointComponent.h"

IMPLEMENT_HIT_PROXY(HWayPointVisProxy, HComponentVisProxy);

void FWaypointVisualizer::OnRegister()
{
	FComponentVisualizer::OnRegister();
	UE_LOG(LogTemp, Warning, TEXT("Waypoint Visualizer Registered"));
}

void FWaypointVisualizer::DrawVisualization(const UActorComponent* Component, const FSceneView* View,
	FPrimitiveDrawInterface* PDI)
{
	FComponentVisualizer::DrawVisualization(Component, View, PDI);
	//Draw the waypoints
	//Draw the lines between the waypoints
	const UEnemeyWaypointComponent* waypointComponent = Cast<UEnemeyWaypointComponent>(Component);
	if (!waypointComponent) return;

	auto waypoints  =waypointComponent->Waypoints;

	for (int i = 0; i < waypoints.Num(); i++)
	{
		FVector waypoint = waypoints[i];
		FVector nextWaypoint = waypoints[(i + 1) % waypoints.Num()];
		PDI->DrawLine(waypoint, nextWaypoint, FLinearColor::Red, SDPG_World, 2.0f);
	
		PDI->SetHitProxy(new HWayPointVisProxy(Component, i));
		PDI->DrawPoint(waypoints[i], FLinearColor::Blue, 20.0f, SDPG_World);
		PDI->SetHitProxy(nullptr);
	}
}

bool FWaypointVisualizer::VisProxyHandleClick(FEditorViewportClient* InViewportClient, HComponentVisProxy* VisProxy,
	const FViewportClick& Click)
{
	if (VisProxy && VisProxy->Component.IsValid())
	{
		WaypointPath.Reset();
		WaypointPath = FComponentPropertyPath(VisProxy->Component.Get());

		
		if (const HWayPointVisProxy* pointProxy = HitProxyCast<HWayPointVisProxy>(VisProxy))
		{
			CurrentSelectedPointIndex = pointProxy->Index;
			return true;
		}
	}

	CurrentSelectedPointIndex = INDEX_NONE;
	return false;
}

bool FWaypointVisualizer::GetWidgetLocation(const FEditorViewportClient* ViewportClient, FVector& OutLocation) const
{
	if(CurrentSelectedPointIndex != INDEX_NONE)
	{
		if (WaypointPath.IsValid())
		{
			UEnemeyWaypointComponent* waypointComponent = Cast<UEnemeyWaypointComponent>(WaypointPath.GetComponent());
			if(waypointComponent == nullptr) return false;

			if(waypointComponent->Waypoints.Num() <= CurrentSelectedPointIndex) return false;
			
			FVector waypoint = waypointComponent->Waypoints[CurrentSelectedPointIndex];
			
			OutLocation = waypoint;
			return true;
		}
	}

	
	return false;
}

bool FWaypointVisualizer::HandleInputDelta(FEditorViewportClient* ViewportClient, FViewport* Viewport,
	FVector& DeltaTranslate, FRotator& DeltaRotate, FVector& DeltaScale)
{

	if (CurrentSelectedPointIndex != INDEX_NONE && WaypointPath.IsValid())
	{
		UEnemeyWaypointComponent* WaypointComponent = Cast<UEnemeyWaypointComponent>(WaypointPath.GetComponent());
		if (WaypointComponent)
		{
			// Update the selected waypoint position by adding DeltaTranslate to the current position
			WaypointComponent->Modify();  // Makes the transaction undoable
			WaypointComponent->Waypoints[CurrentSelectedPointIndex] += DeltaTranslate;
            
			// Mark the component as changed
			WaypointComponent->MarkRenderStateDirty();
			return true;
		}
	}
	
	return false;
}


