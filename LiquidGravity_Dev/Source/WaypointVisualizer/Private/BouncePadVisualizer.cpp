#include "BouncePadVisualizer.h"

#include "LiquidGravity_Dev/Components/BouncePadComponent.h"

void FBouncePadVisualizer::OnRegister()
{
	FComponentVisualizer::OnRegister();
}

void FBouncePadVisualizer::DrawVisualization(const UActorComponent* Component, const FSceneView* View,
	FPrimitiveDrawInterface* PDI)
{
	FComponentVisualizer::DrawVisualization(Component, View, PDI);


	const UBouncePadComponent* bouncePadComponent = Cast<UBouncePadComponent>(Component);
	if(!bouncePadComponent) return;

	const FVector padImpulseVector = bouncePadComponent->GetImpulseDirection().GetSafeNormal();
	const FVector ForwardVector = padImpulseVector.GetSafeNormal();
	FVector UpVector = FVector::UpVector; // Default Up direction in world space
	const FVector RightVector = FVector::CrossProduct(UpVector, ForwardVector).GetSafeNormal();
	UpVector = FVector::CrossProduct(ForwardVector, RightVector).GetSafeNormal();


	const FMatrix Matrix = FMatrix
	(
		FPlane(ForwardVector, 0.0f),  // X axis
		FPlane(RightVector, 0.0f),    // Y axis
		FPlane(UpVector, 0.0f),       // Z axis
		FPlane(FVector::ZeroVector, 1.0f) // Translation, set to origin
	);


	const FMatrix FinalMatrix = FScaleRotationTranslationMatrix(
	{1,1,1},
	Matrix.Rotator(),             // Convert the matrix to a rotator
	bouncePadComponent->GetComponentLocation());

	const FLinearColor Color = FLinearColor::Yellow;

	constexpr float Length = 300.f;
	constexpr float Width = 10.f;
	constexpr float Thickness = 8.f;

	
	DrawDirectionalArrow(PDI, FinalMatrix, Color, Length, Width, SDPG_World, Thickness);
}
