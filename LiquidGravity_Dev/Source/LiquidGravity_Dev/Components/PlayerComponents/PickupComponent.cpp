#include "PickupComponent.h"

#include "CameraHandler.h"
#include "CombatComponent.h"
#include "LiquidMath.h"
#include "LiquidMovementComponent.h"
#include "PlayerFeedbackComponent.h"
#include "LiquidGravity_Dev/Actors/ThrowableActor.h"
#include "LiquidGravity_Dev/Core/LiquidCamera.h"
#include "LiquidGravity_Dev/Core/LiquidPlayer.h"

UPickupComponent::UPickupComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UPickupComponent::BeginPlay()
{
	Super::BeginPlay();
	DisableWidget();

	LiquidPlayer = Cast<ALiquidPlayer>(GetOwner());
	check(LiquidPlayer);

	CombatComponent = LiquidPlayer->CombatComponent;
	check(CombatComponent);
}

void UPickupComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	// Only tick when we are in gameplay mode
	if (!GetWorld() || !GetWorld()->IsGameWorld()) return;

	if(CombatComponent->HasPickedUpActor() && IsValid(LiquidPlayer->CombatComponent->GetPickedUpActor()))
	{
		DisableWidget();
		//Transform and rotate using sine
		FVector newPos = FVector(25.f, 35.f, 25.f) * FMath::Sin(PickUpAnimatorTimer * 2) + LiquidPlayer->GetActorLocation() + (LiquidPlayer->GetActorUpVector() * (120.f + LiquidPlayer->CombatComponent->GetPickedUpActor()->LeftUpDelta.Y) -
				LiquidPlayer->GetActorRightVector() * (60.f + LiquidPlayer->CombatComponent->GetPickedUpActor()->LeftUpDelta.X) );
		FRotator newRot = FRotator(45.f, 45.f, 45.f) * FMath::Sin(PickUpAnimatorTimer * 2);
		PickUpAnimatorTimer += DeltaTime;

		FVector lerpedPos = FLiquidMath::LerpSmooth(LiquidPlayer->CombatComponent->GetPickedUpActor()->GetActorLocation(), newPos, DeltaTime, 0.5f, 0.01f);
		FRotator lerpedRot = FLiquidMath::SlerpSmooth(LiquidPlayer->CombatComponent->GetPickedUpActor()->GetActorRotation().Quaternion(), newRot.Quaternion(), DeltaTime, 1.f, 0.01f).Rotator();
		
		//TODO: Interp these positions
		
		LiquidPlayer->CombatComponent->GetPickedUpActor()->SetActorLocationAndRotation(lerpedPos, lerpedRot);

		return;
	}

	
	//Don't do anything if we are already pulling
	if(bIsPulling)
	{
		Pull(DeltaTime);
		return;
	}
	
	ALiquidCamera* liquidCamera = LiquidPlayer->CameraHandler->LiquidCamera;
	check(liquidCamera)

	FuzzySelectStruct fuzzySelectStruct = FuzzySelectStruct();
	fuzzySelectStruct.SelectionType = FuzzySelectStruct::Class;
	fuzzySelectStruct.SubClass = AThrowableActor::StaticClass();
	const FHitResult* enemyHit = liquidCamera->GetFuzzySelection(fuzzySelectStruct);
	if (enemyHit != nullptr)
	{
		if(enemyHit->GetActor())
		{
			EnableWidget(enemyHit->ImpactPoint, Cast<AThrowableActor>(enemyHit->GetActor()));
		}else
		{
			DisableWidget();
		}
	}else
	{
		DisableWidget();
	}
}

void UPickupComponent::Pickup()
{
	//If we locked on to an object, and we are not already pulling or have one in our hand
	if(bIsLockingOn && !CombatComponent->HasPickedUpActor())
	{
		LiquidPlayer->FeedbackComponent->OnCombatObjectPulled.Broadcast();
		PullTimer = 0.f;
		EnablePulling();
	}
}

void UPickupComponent::EnableWidget(const FVector& impactLocation, AThrowableActor* actor)
{
	if(actor->GetIsThrown()) return;
	
	bIsLockingOn = true;
	LockOnTarget = actor;
	LockingOnLocation = impactLocation;
	SetVisibility(true);
	SetWorldLocation(impactLocation);	
}

void UPickupComponent::DisableWidget()
{
	LockOnTarget = nullptr;
	bIsLockingOn = false;
	SetVisibility(false);
}

void UPickupComponent::EnablePulling()
{
	LockOnTarget->SetPull();
	bIsPulling = true;
	PullingLocation = LiquidPlayer->SkeletalMeshComponent->GetSocketLocation("LeftThrowingSocket");
}

void UPickupComponent::Pull(float deltaTime)
{
	SetVisibility(false);

	if(!IsValid(LockOnTarget)) return;
	
	FVector currentLocation = LockOnTarget->GetActorLocation();
	FRotator currentRotation = LockOnTarget->GetActorRotation();

	LockOnTarget->SetIsPickedUp(true);
	
	if(PullTimer >= TimeToPull-1.3f)
	{
		FinishPull();
		return;
	}

	PullingLocation = LiquidPlayer->SkeletalMeshComponent->GetSocketLocation("LeftThrowingSocket");

	//FVector smoothLocation = FLiquidMath::LerpSmooth(currentLocation, PullingLocation, deltaTime, 2.f, 0.01f);
	FVector smoothLocation = FMath::Lerp(currentLocation, PullingLocation, PullTimer/TimeToPull);
	FRotator smoothRotation = FMath::Lerp(currentRotation, LiquidPlayer->GetActorRotation(), PullTimer/TimeToPull);
	PullTimer += deltaTime;
	LockOnTarget->SetActorLocationAndRotation(smoothLocation,smoothRotation);
}

void UPickupComponent::FinishPull()
{
	bIsPulling = false;

	CombatComponent->Pickup(LockOnTarget);
	//LockOnTarget->AttachToComponent(LiquidPlayer->SkeletalMeshComponent, FAttachmentTransformRules::KeepWorldTransform, "LeftThrowingSocket");
	PickupInitialPosition = LockOnTarget->GetActorLocation()  - LiquidPlayer->GetActorLocation() - LiquidPlayer->GetActorUpVector() * 140.f + LiquidPlayer->GetActorRightVector() * 275.f;
	PickupInitialRotation = LockOnTarget->GetActorRotation() - LiquidPlayer->GetActorRotation();
	PickUpAnimatorTimer = 0.f;

	LockOnTarget->SetIsPickedUp(true);

	LockOnTarget = nullptr;
}

