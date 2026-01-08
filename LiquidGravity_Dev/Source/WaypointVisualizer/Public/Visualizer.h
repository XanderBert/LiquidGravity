#pragma once
#include "ComponentVisualizer.h"



struct HWayPointVisProxy : public HComponentVisProxy
{
	DECLARE_HIT_PROXY();

	HWayPointVisProxy (const UActorComponent* InComponent, int InIndex)
		: HComponentVisProxy(InComponent, HPP_Wireframe)
		, Index(InIndex)
	{}

	int Index;
};

class FWaypointVisualizer : public FComponentVisualizer
{
public:
	virtual void OnRegister() override;
	virtual void DrawVisualization(const UActorComponent* Component, const FSceneView* View, FPrimitiveDrawInterface* PDI) override;
	virtual bool VisProxyHandleClick(FEditorViewportClient* InViewportClient, HComponentVisProxy* VisProxy, const FViewportClick& Click) override;
	//virtual void EndEditing() override;
	virtual bool GetWidgetLocation(const FEditorViewportClient* ViewportClient, FVector& OutLocation) const override;
	// virtual bool GetCustomInputCoordinateSystem(const FEditorViewportClient* ViewportClient, FMatrix& OutMatrix) const override;
	virtual bool HandleInputDelta(FEditorViewportClient* ViewportClient, FViewport* Viewport, FVector& DeltaTranslate, FRotator& DeltaRotate, FVector& DeltaScale) override;
	//virtual bool HandleInputKey(FEditorViewportClient* ViewportClient, FViewport* Viewport, FKey Key, EInputEvent Event) override;
	//virtual TSharedPtr<SWidget> GenerateContextMenu() const override;
	// virtual void DrawVisualizationHUD(const UActorComponent* Component, const FViewport* Viewport, const FSceneView* View, FCanvas* Canvas) override;
	//virtual bool IsVisualizingArchetype() const override;
private:
	FComponentPropertyPath WaypointPath{};
	int CurrentSelectedPointIndex = INDEX_NONE;
};


