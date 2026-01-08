#include "GrindTask.h"

#include "LiquidMath.h"
#include "Components/CapsuleComponent.h"
#include "Framework/Application/AnalogCursor.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "LiquidGravity_Dev/Components/EnergyComponent.h"
#include "LiquidGravity_Dev/Components/PlayerComponents/LiquidMovementComponent.h"
#include "LiquidGravity_Dev/Components/PlayerComponents/PlayerFeedbackComponent.h"
#include "LiquidGravity_Dev/Components/PlayerComponents/PlayerStuntComponent.h"
#include "LiquidGravity_Dev/Core/LiquidGameInstance.h"
#include "LiquidGravity_Dev/Core/LiquidPlayer.h"
#include "LiquidGravity_Dev/DataTables/EnergyAdditionDataTable.h"

EStateTreeRunStatus UGrindTask::Tick(FStateTreeExecutionContext& Context, const float DeltaTime)
{
	Super::Tick(Context, DeltaTime);

	TimeSinceEnergyAddition += DeltaTime;
	const FEnergyAdditionDataTable* energyAddition = EnergyAdditionDataTable->FindRow<FEnergyAdditionDataTable>(FName("Grind"),FString());

	//Gain Energy
	if (TimeSinceEnergyAddition > energyAddition->TimeInterval)
	{
		LiquidPlayer->EnergyComponent->GainEnergy(energyAddition->EnergyAddition);
		TimeSinceEnergyAddition -= energyAddition->TimeInterval;
	}
	
	MovementComponent->StartTaskTick();
	LiquidPlayer->LiquidMovementComponent->RemoveWallRidingTag();
	if (LiquidPlayer->LiquidMovementComponent->bGrindDirectionChangeIntended)
	{
		bEnteredForward = !bEnteredForward;
		LiquidPlayer->LiquidMovementComponent->bGrindDirectionChangeIntended = false;
		Speed = -Speed;
		AngleRotated = -AngleRotated;
		LiquidPlayer->FeedbackComponent->OnGrindDirectionChange.Broadcast();
	}
	
	FVector directionToGrind = SplineToGrindOn->GetLocationAtDistanceAlongSpline(TraveledDistance, ESplineCoordinateSpace::World);
	
	
	if (bEnteredForward)
	{
		directionToGrind -= SplineToGrindOn->GetLocationAtDistanceAlongSpline(TraveledDistance - 10.f, ESplineCoordinateSpace::World);
	}
	else
	{
		directionToGrind -= SplineToGrindOn->GetLocationAtDistanceAlongSpline(TraveledDistance + 10.f, ESplineCoordinateSpace::World);
	}

	FVector2D playerInput = MovementComponent->GetInputMovementVector();
	FVector xAxis, yAxis, zAxis;
	MovementComponent->GetPlayerInputSpaceAxes(xAxis,yAxis,zAxis);

	FVector intendedInput = xAxis * playerInput.Y + yAxis * playerInput.X;

	intendedInput.Normalize();
	directionToGrind.Normalize();

	float dot = intendedInput.Dot(directionToGrind);
	
	float baseMultiplier = 1.f;
	baseMultiplier += LiquidPlayer->StuntComponent->GetCurrentMultiplier() * 0.2f;

	TargetSpeed += dot * GrindAcceleration * DeltaTime;

	TargetSpeed = FMath::Clamp(TargetSpeed, MinSpeed, LiquidPlayer->LiquidMovementComponent->GetMaxSpeed() * baseMultiplier);
	
	Speed = FLiquidMath::MoveTowards(Speed, TargetSpeed, LiquidPlayer->LiquidMovementComponent->GetMaxSpeed() *baseMultiplier * DeltaTime);
	
	
	const FVector splineUpAxis = SplineToGrindOn->GetUpAxisAtDistance(TraveledDistance);

	FVector movementDirection = SplineToGrindOn->GetTangentAtDistanceAlongSpline(TraveledDistance,ESplineCoordinateSpace::World);
	
	if (!bEnteredForward)
	{
		movementDirection *= -1;
	}

	if (movementDirection.Dot(directionToGrind) < 0.f)
	{
		movementDirection *= -1;
	}
	const FVector orientedUpAxis = splineUpAxis.RotateAngleAxis(AngleRotated, movementDirection.GetSafeNormal());

	
	FVector nextGrindLocation = SplineToGrindOn->GetNextGrindLocationTask(TraveledDistance, orientedUpAxis, Speed * DeltaTime);

	//Reset traveled distance cause player changed direction
	if (Speed < 0)
	{
		TraveledDistance = SplineToGrindOn->GetDistanceAlongSplineAtLocation(nextGrindLocation,ESplineCoordinateSpace::World);
	}
	
	//Add the distance traveled to the player
	if(bEnteredForward) TraveledDistance += Speed * DeltaTime;
	else				TraveledDistance -= Speed * DeltaTime;
	


	
	const FVector directionToGrind2 = directionToGrind.GetSafeNormal();
	const FVector2D movementVector = MovementComponent->GetInputMovementVector();

	if (SplineToGrindOn->HasCylindricalRotation())
	{
		AngleRotated -= movementVector.X * RotationSpeed * DeltaTime;
	}
	FString cleanRot = FString::SanitizeFloat(AngleRotated);
		
	MovementComponent->LookDirection = directionToGrind2;
	MovementComponent->RotateToLookDirection(DeltaTime, 1.f, 0.01f);
	
	
	LiquidPlayer->LiquidMovementComponent->OverridenGrindingVelocity = Speed * FVector(1,0,0);


	//If we are jumping we should exit the grind
	//If we are at the end of the spline we can exit the grind
	const bool bIsAtEndOrBeginningOfSpline =  (TraveledDistance >= (SplineDistance)  || TraveledDistance <= 0) && !bJustEntered;

	const bool bWantsToJump = LiquidPlayer->LiquidMovementComponent->HasJumpTag();
	const bool bWantsToDodge = LiquidPlayer->LiquidMovementComponent->IsDodging();
	const bool bWantsToChangeGravity = LiquidPlayer->LiquidMovementComponent->HasChangeGravityTag();

	bool bExitSpline = false;
	
	if (bIsAtEndOrBeginningOfSpline)
	{
		if (SplineToGrindOn->IsEndless())
		{
			if (bEnteredForward)
			{
				nextGrindLocation = SplineToGrindOn->GetSplinePointAtStartEnd(true, orientedUpAxis);
				TraveledDistance = SplineToGrindOn->GetDistanceAlongSplineAtLocation(nextGrindLocation, ESplineCoordinateSpace::World);
			}
			else
			{
				nextGrindLocation = SplineToGrindOn->GetSplinePointAtStartEnd(false, orientedUpAxis);
				TraveledDistance = SplineToGrindOn->GetDistanceAlongSplineAtLocation(nextGrindLocation, ESplineCoordinateSpace::World);
			}
		}
		else
		{
			bExitSpline = true;
		}
	}

	MovementComponent->SetLockGravity(false);

	LiquidPlayer->SetActorLocation(nextGrindLocation, false, nullptr, ETeleportType::ResetPhysics);

	//TODO: Smooth this out without breaking rest of the code
	MovementComponent->ApplyTaskTick(DeltaTime,0);

	{
			
		//TODO: Remove constants
		LiquidPlayer->LiquidMovementComponent->SetForcedGravity(
			((orientedUpAxis
			* -1 * LiquidPlayer->LiquidMovementComponent->GetGravityStrength())));

		//TODO: should this be removed??
		LiquidPlayer->LiquidMovementComponent->SetGravityAlignment(UKismetMathLibrary::MakeRotFromZX(orientedUpAxis, movementDirection).Quaternion());
	}
	
	if(bWantsToJump || bWantsToDodge || bExitSpline || bWantsToChangeGravity)
	{
		if (bWantsToJump)
		{
			LiquidPlayer->LiquidMovementComponent->SetJumpingFromSpline(true);
		}
		
		
		//TODO: Remove this from Movement Component
		MovementComponent->DisableGrindingTag();

		FVector intendedTaskVelocity = (Speed *
			SplineToGrindOn->GetTangentAtDistanceAlongSpline(TraveledDistance,ESplineCoordinateSpace::World).GetSafeNormal()
			);

		if (!bEnteredForward)
		{
			intendedTaskVelocity *= -1;
		}
		
		//if (SplineToGrindOn->HasCylindricalRotation())
		
/*
		else
		{
			
			FVector upVector = SplineToGrindOn->GetUpAxisAtDistance(TraveledDistance);
			LiquidPlayer->LiquidMovementComponent->SetForcedGravity(
				((upVector
				* -1 * LiquidPlayer->LiquidMovementComponent->GetGravityStrength())));
				LiquidPlayer->LiquidMovementComponent->SetGravityAlignment(UKismetMathLibrary::MakeRotFromZX(upVector, movementDirection).Quaternion());
		}*/
		
		LiquidPlayer->LiquidMovementComponent->StartTaskTick();
		LiquidPlayer->LiquidMovementComponent->SetTaskVelocity(intendedTaskVelocity,DeltaTime);
		LiquidPlayer->LiquidMovementComponent->ApplyTaskTick(DeltaTime);
		
		return EStateTreeRunStatus::Succeeded;
	}

	
	bJustEntered = false;
	ResetTimeSinceLastGrind();
	
	return EStateTreeRunStatus::Running;

}

EStateTreeRunStatus UGrindTask::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
	Super::EnterState(Context, Transition);
	
	Speed = LiquidPlayer->CapsuleComponent->GetPhysicsLinearVelocity().Length();

	
	SplineToGrindOn = LiquidPlayer->LiquidMovementComponent->GetCurrentRail();

	const FVector actorEnterLocation = LiquidPlayer->GetActorLocation();
	FVector actorVelocity = MovementComponent->GetVelocity();
	
	float alignedSpeed = actorVelocity.Dot(LiquidPlayer->GetGravityDirection());
	if (alignedSpeed > 0.f)
	{
		float gravityDirectionComponent = actorVelocity.Dot(LiquidPlayer->GetGravityDirection());
		actorVelocity = actorVelocity - gravityDirectionComponent * LiquidPlayer->GetGravityDirection();
	}
	
	actorVelocity.Normalize();

	
	const FVector nextGrindLocation = SplineToGrindOn->GetNextGrindLocation(actorEnterLocation, actorVelocity, SplineToGrindOn->GetUpAxisAtDistance(TraveledDistance));
	TraveledDistance = SplineToGrindOn->GetDistanceAlongSplineAtLocation(nextGrindLocation, ESplineCoordinateSpace::World);


	//Direction Check
	FVector movementDirection = SplineToGrindOn->GetTangentAtDistanceAlongSpline(TraveledDistance,ESplineCoordinateSpace::World);
	
	if (movementDirection.Dot(LiquidPlayer->GetActorForwardVector()) > 0)
	{
		bEnteredForward = true;
	}
	else
	{
		bEnteredForward = false;
		movementDirection *= -1;
	}
	
	SplineDistance = SplineToGrindOn->GetSplineLength();
	bJustEntered = true;

	if (SplineToGrindOn->HasCylindricalRotation())
	{
		FVector splineUpVector = SplineToGrindOn->GetUpAxisAtDistance(TraveledDistance);
		FRotator initialRotator = UKismetMathLibrary::MakeRotFromXZ(movementDirection, splineUpVector);
		FVector playerInitialUpVector = initialRotator.Quaternion() * FVector::UpVector;
		//FVector splineUpVector = SplineToGrindOn->GetUpAxisAtDistance(TraveledDistance);

		float resultAngleInRadians;
		playerInitialUpVector.Normalize();
		splineUpVector.Normalize();

		auto crossProduct = playerInitialUpVector.Cross(splineUpVector);
		auto dotProduct = playerInitialUpVector.Dot(splineUpVector);

		if(crossProduct.Z > 0 )
		{
			resultAngleInRadians = acosf(dotProduct);
		}
		else
		{
			resultAngleInRadians = -1 * acosf(dotProduct);
		}

		AngleRotated = FMath::RadiansToDegrees(resultAngleInRadians);
	}
	else
	{
		AngleRotated = 0;
	}

	if (OnGrindStayForceFeedback)
	{
		ULiquidGameInstance* GameInstance = Cast<ULiquidGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
		
		if (GameInstance->VibrationEnabled)
		{
			//Play force feedback
			FForceFeedbackParameters params;
			params.Tag = "Grind";
			params.bLooping = true;
			UGameplayStatics::GetPlayerController(GetWorld(), 0)->ClientPlayForceFeedback(OnGrindStayForceFeedback, params);
		}
	}


	//BroadCast for feedback
	LiquidPlayer->FeedbackComponent->OnGrind.Broadcast();

	//Stunt system
	FStuntStruct GrindStruct;
	GrindStruct.StuntName = "Grinding";
	GrindStruct.ScoreAddition = 500.f;
	GrindStruct.contextActor = SplineToGrindOn->GetOwner();
	LiquidPlayer->StuntComponent->AddStuntStruct(GrindStruct);

	return EStateTreeRunStatus::Running;
}

void UGrindTask::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
	Super::ExitState(Context, Transition);

	if (OnGrindStayForceFeedback)
	{
		//Play force feedback
		if (IsValid(UGameplayStatics::GetPlayerController(GetWorld(),0)))
		{
			ULiquidGameInstance* gameInstance = Cast<ULiquidGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
			if (gameInstance->VibrationEnabled)
			{
				UGameplayStatics::GetPlayerController(GetWorld(), 0)->ClientStopForceFeedback(OnGrindStayForceFeedback, "Grind");
			}
		}

		LiquidPlayer->FeedbackComponent->OnGrindEnded.Broadcast();
	}
}

void UGrindTask::ResetTimeSinceLastGrind() const
{
	LiquidPlayer->LiquidMovementComponent->TimeSinceLastGrind = 0;
}

