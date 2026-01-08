#include "PlayerHookingComponent.h"

#include "CameraHandler.h"
#include "Blueprint/UserWidget.h"
#include "Components/Widget.h"
#include "Kismet/GameplayStatics.h"
#include "LiquidGravity_Dev/Actors/HookableActor.h"
#include "LiquidGravity_Dev/Core/LiquidCamera.h"
#include "LiquidGravity_Dev/Core/LiquidPlayer.h"

UPlayerHookingComponent::UPlayerHookingComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


void UPlayerHookingComponent::BeginPlay()
{
	Super::BeginPlay();
	LiquidPlayer = Cast<ALiquidPlayer>(GetOwner());
	DisableWidget();
}


void UPlayerHookingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Only tick when we are in gameplay mode
	if (!GetWorld() || !GetWorld()->IsGameWorld()) return;

	ALiquidCamera* liquidCamera = Cast<ALiquidPlayer>(GetOwner())->CameraHandler->LiquidCamera;
	check(liquidCamera)
	
	FuzzySelectStruct fuzzySelectStruct = FuzzySelectStruct();
	fuzzySelectStruct.SelectionType = FuzzySelectStruct::Class;
	fuzzySelectStruct.SubClass = AHookableActor::StaticClass();
	
	const FHitResult* hookHit = liquidCamera->GetFuzzySelection(fuzzySelectStruct);
	if (hookHit != nullptr)
	{
		if(hookHit->GetActor())
		{
			EnableWidget(hookHit->ImpactPoint);
			HookActor = Cast<AHookableActor>(hookHit->GetActor());
			return;
		}
	}
	HookActor = nullptr;
	DisableWidget();
}

AHookableActor* UPlayerHookingComponent::GetTargetHook() const
{
	return HookActor;
}

void UPlayerHookingComponent::TryHook() const
{
	if (!IsValid(HookActor))
	{
		return;
	}
	OnHookStarted.Broadcast();
	LiquidPlayer->GameplayTags.AddTag(HookTag);
}

void UPlayerHookingComponent::RemoveHookTag() const
{
	LiquidPlayer->GameplayTags.RemoveTag(HookTag);
}
void UPlayerHookingComponent::RenderHookWidget() const
{
	if(!HookWidget) return;

	FuzzySelectStruct fuzzySelectStruct = FuzzySelectStruct();
	fuzzySelectStruct.SelectionType = FuzzySelectStruct::Component;
	
	const FHitResult* hit = LiquidPlayer->CameraHandler->LiquidCamera->GetFuzzySelection(fuzzySelectStruct);

	if (hit != nullptr)
	{
		if(hit->bBlockingHit)
		{
			FVector widgetLocation = hit->ImpactPoint;
		
			//world to screen space
			FVector2d screenLocation;
			UGameplayStatics::ProjectWorldToScreen(UGameplayStatics::GetPlayerController(GetWorld(), 0), widgetLocation, screenLocation);


			HookWidget->SetVisibility(ESlateVisibility::Visible);
			HookWidget->SetPositionInViewport(FVector2D(screenLocation.X, screenLocation.Y));
			return;
		}
	}
	
	HookWidget->SetVisibility(ESlateVisibility::Hidden);
}

void UPlayerHookingComponent::EnableWidget(const FVector& impactPoint)
{
	SetVisibility(true);
	SetWorldLocation(impactPoint);
}

void UPlayerHookingComponent::DisableWidget()
{
	SetVisibility(false);
}