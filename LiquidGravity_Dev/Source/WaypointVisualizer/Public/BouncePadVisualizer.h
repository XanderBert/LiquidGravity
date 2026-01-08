#pragma once
#include "ComponentVisualizer.h"

class FBouncePadVisualizer : public FComponentVisualizer
{
public:
	virtual void OnRegister() override;
	virtual void DrawVisualization(const UActorComponent* Component, const FSceneView* View, FPrimitiveDrawInterface* PDI) override;
};
