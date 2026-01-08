#include "LiquidMovementComponent.h"

#include "CameraHandler.h"
#include "InputActionValue.h"
#include "LiquidMath.h"
#include "PlayerStuntComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "LiquidGravity_Dev/Actors/ThrowableActor.h"
#include "LiquidGravity_Dev/Actors/Enemies/BaseEnemy.h"
#include "LiquidGravity_Dev/Core/LiquidCamera.h"
#include "LiquidGravity_Dev/Core/LiquidGameInstance.h"
#include "LiquidGravity_Dev/Core/LiquidPlayer.h"

ULiquidMovementComponent::ULiquidMovementComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bTickEvenWhenPaused = true;
}

void ULiquidMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	// Calculate the minimum ground dot product based on the max  angle we allow
	MinGroundDotProduct = FMath::Cos(FMath::DegreesToRadians(MaxGroundAngle));

	// Calculate the wall dot product
	WallDotProductMinMax = FMath::Cos(FMath::DegreesToRadians(90-WallAngleDelta));

	LiquidPlayer = Cast<ALiquidPlayer>(GetOwner());

	LiquidGameInstance = Cast<ULiquidGameInstance>(GetWorld()->GetGameInstance());

	//Set the initial values so we can dodge and grind on the first frame
	TimeSinceLastDodge = SecondsUntilWeCanDodge;
	TimeSinceLastGrind = SecondsUntilWeCanGrind;


	// If the owner has a primitive component, bind the OnHit event
	if (UCapsuleComponent* PrimitiveComponent = LiquidPlayer->GetComponentByClass<UCapsuleComponent>())
	{
		PlayerCapsule = PrimitiveComponent;
	}
	verify(PlayerCapsule);
}

#pragma region INPUT

void ULiquidMovementComponent::SetTaskSmoothRotation(const FQuat& targetRotation, float deltaTime, float rotationTime,
                                                     float precision)
{
	RotationToApply = (FLiquidMath::SlerpSmooth(LiquidPlayer->GetActorRotation().Quaternion(), targetRotation, deltaTime, rotationTime, 0.01f)).Rotator();
}

void ULiquidMovementComponent::HandleMovementInput(const FInputActionValue& Value)
{
	InputMovementVector = Value.Get<FVector2D>();

	//Clamp the movement vector to a maximum size of 1 so diagonal movement is not faster
	FVector movementVector = FVector(InputMovementVector.X, InputMovementVector.Y, 0.0f).GetClampedToMaxSize2D(1.0f);
	InputMovementVector = FVector2D(movementVector.X, movementVector.Y);
}

void ULiquidMovementComponent::HandleInteractInput()
{
	if (WallContactCount > 0)
	{
		AddWallRidingTag();
	}

	if (IsGrinding())
	{
		bGrindDirectionChangeIntended = true;
	}
}

#pragma endregion

#pragma region GETTERS and SETTERS

void ULiquidMovementComponent::AddTaskVelocityChange(const FVector& velocityChange, bool isWorldSpace)
{
	if (isWorldSpace)
	{
		CurrentVelocity += velocityChange;
	}
	else
	{
		FVector xAxis, yAxis, zAxis;
		GetPlayerInputSpaceAxes(xAxis, yAxis,zAxis);

		CurrentVelocity += xAxis * velocityChange.X + yAxis * velocityChange.Y + zAxis * velocityChange.Z;
	}
}


void ULiquidMovementComponent::SetTaskVelocity(const FVector& velocityChange, float deltaTime, bool isWorldSpace)
{
	if (isWorldSpace)
	{
		CurrentVelocity = velocityChange;
	}
	else
	{
		FVector xAxis, yAxis, zAxis;
		GetPlayerInputSpaceAxes(xAxis, yAxis, zAxis);
		CurrentVelocity = xAxis * velocityChange.X + yAxis * velocityChange.Y + zAxis * velocityChange.Z;
	}
}

FVector ULiquidMovementComponent::GetPlayerFootLocation() const
{
	FVector newVector = LiquidPlayer->GetActorLocation() - (GetCurrentUpAxis() * FootDisplacement);
	return newVector;
}

void ULiquidMovementComponent::SetLockGravity(bool bNewLockGravity, bool bForceReset)
{
	if (bNewLockGravity == true && (bLockGravity == false || bForceReset))
	{
		bLockGravity = true;
		LockedGravity = GetGravityAtPlayerPosition();
		LockedGravityOrientation = GravityAlignment;
	}
	else if (bNewLockGravity == false && bLockGravity == true)
	{
		bLockGravity = false;
		LiquidGameInstance->Gravity.SetIsForcedLockedGravity(false, FVector::Zero());
	}
}

void ULiquidMovementComponent::SetForcedGravity(const FVector& newGravity)
{
	LiquidGameInstance->Gravity.SetIsForcedLockedGravity(true, newGravity);
	GravityAlignment = LiquidGameInstance->Gravity.GetGravityAlignment(LiquidPlayer->GetActorLocation(), GravityAlignment);
	//SetLockGravity(true, true);
}

bool ULiquidMovementComponent::GetLockGravity() const
{
	if (LiquidGameInstance->Gravity.GetIsForcedGravity())
	{
		return false;
	}
	return bLockGravity;
}

FQuat ULiquidMovementComponent::GetLockGravityOrientation() const
{
	return LockedGravityOrientation;
}

UBouncePadComponent* ULiquidMovementComponent::GetBouncePad()
{
	return BouncePad;
}

UGrindSplineComponent* ULiquidMovementComponent::GetCurrentRail() const
{
	return CurrentRail;
}

void ULiquidMovementComponent::GetPlayerInputSpaceAxes(FVector& XAxis, FVector& YAxis, FVector& ZAxis) const
{
	XAxis = FLiquidMath::ProjectOnContactPlane(CurrentForwardAxis, ContactNormal).GetSafeNormal();
	YAxis = FLiquidMath::ProjectOnContactPlane(CurrentRightAxis, ContactNormal).GetSafeNormal();
	ZAxis = FLiquidMath::ProjectOnContactPlane(CurrentUpAxis, ContactNormal).GetSafeNormal();
}

void ULiquidMovementComponent::SetBouncePad(UBouncePadComponent* UBouncePad)
{
	BouncePad = UBouncePad;
}

void ULiquidMovementComponent::SetSnapToGround(bool value)
{
	bSnapToGround = value;
}

bool ULiquidMovementComponent::IsVelocityWorldSpace() const
{
	return bIsDesiredVelocityWorldSpace;
}

void ULiquidMovementComponent::SetVelocityWorldSpace(bool value)
{
	bIsDesiredVelocityWorldSpace = value;
}

bool ULiquidMovementComponent::IsStunting() const
{
	if (LiquidPlayer->GameplayTags.HasTag(LiquidPlayer->StuntComponent->GetStuntTag()))
		return true;
	
	return false;
}


bool ULiquidMovementComponent::GetSnapToGround()
{
	if (!bSnapToGround)
	{
		return false;
	}


	if (StepsSinceLastGrounded > 1 || StepsSinceLastJump <= 2)
	{
		return false;
	}
	const float speed = CurrentVelocity.Size();
	if (speed > MaxSnapSpeed)
	{
		return false;
	}

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(GetOwner());

	const FVector Start = LiquidPlayer->GetActorLocation();
	const FVector End = -CurrentUpAxis * 130.0f + Start;
	if (!GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params))
	{
		return false;
	}

	const float upDot = CurrentUpAxis.Dot(Hit.Normal);
	if (upDot < MinGroundDotProduct)
	{
		return false;
	}

	GroundContactCount = 1;
	ContactNormal = Hit.Normal;

	//Adjust the velocity towards the ground
	const float dot = CurrentVelocity.Dot(Hit.Normal);

	//TODO: This could bug out jumping more testing needs to be done
	if (dot > 0.0)
	{
		CurrentVelocity -= Hit.Normal * 1.5f;
	}

	return true;
}

void ULiquidMovementComponent::ApplyRotation(const FRotator& rotation)
{
	RotationToApply = rotation;
}

FVector2D ULiquidMovementComponent::GetInputMovementVector() const
{
	return InputMovementVector;
}

void ULiquidMovementComponent::SetDesiredVelocity(const FVector& newDesiredVelocity, bool isWorldSpace)
{
	DesiredVelocity = newDesiredVelocity;
	bIsDesiredVelocityWorldSpace = isWorldSpace;
}

FQuat ULiquidMovementComponent::GetGravityAlignment() const
{
	return GravityAlignment;
}

void ULiquidMovementComponent::SetGravityAlignment(const FQuat& newGravityAlignment)
{
	GravityAlignment = newGravityAlignment;
}

float ULiquidMovementComponent::GetActorRotationSpeed() const
{
	return ActorRotationSpeed;
}


void ULiquidMovementComponent::LockGravityFlipFlop()
{
	SetLockGravity(!GetLockGravity());
}

bool ULiquidMovementComponent::IsDodging()
{
	return LiquidPlayer->GameplayTags.HasTag(DodgingTag);
}

bool ULiquidMovementComponent::IsWallRiding()
{
	return false;

	// no more wallriding
	if (!LiquidPlayer)
		return false;
	return LiquidPlayer->GameplayTags.HasTag(WallRidingTag);
}

bool ULiquidMovementComponent::IsGrinding() const
{
	if (!LiquidPlayer)
		return false;
	return LiquidPlayer->GameplayTags.HasTag(GrindingTag);
}


bool ULiquidMovementComponent::IsGrounded() const
{
	if (!IsGrinding())
	return GroundContactCount > 0;
	else
	{
		return false;
	}
}

bool ULiquidMovementComponent::IsOnWall() const
{
	return WallContactCount > 0;
}


bool ULiquidMovementComponent::OnSteepContact() const
{
	return SteepContactCount > 0;
}

float ULiquidMovementComponent::GetMaxSpeed() const
{
	return MaxHighSpeedSpeed;
}

FVector ULiquidMovementComponent::GetVelocity() const
{
	return CurrentVelocity;
}

FVector ULiquidMovementComponent::GetDisplayVelocity() const
{
	if (IsGrinding())
		return OverridenGrindingVelocity;

	FVector xAxis, yAxis, zAxis;
	GetPlayerInputSpaceAxes(xAxis,yAxis,zAxis);

	FVector velocity = xAxis.Dot(CurrentVelocity) * xAxis + yAxis.Dot(CurrentVelocity) * yAxis;
	return velocity;
}

FVector ULiquidMovementComponent::GetContactNormal() const
{
	return ContactNormal;
}

FVector ULiquidMovementComponent::GetWallNormal() const
{
	return WallNormal;
}

FVector ULiquidMovementComponent::GetSteepNormal() const
{
	return SteepNormal;
}

int ULiquidMovementComponent::GetMaxAirJumps() const
{
	return MaxAirJumps;
}

void ULiquidMovementComponent::ResetTicksSinceLastJump()
{
	StepsSinceLastJump = 0;
}

void ULiquidMovementComponent::IncreaseTicksSinceLastJump()
{
	++StepsSinceLastJump;
}

float ULiquidMovementComponent::GetJumpHeight() const
{
	return JumpHeight;
}

float ULiquidMovementComponent::GetGravityStrength() const
{
	return GravityAtPlayerPosition.Length();
}

void ULiquidMovementComponent::SetJumpingFromSpline(bool value)
{
	bJumpingFromSpline = value;
}

FVector ULiquidMovementComponent::GetCurrentUpAxis() const
{
	return CurrentUpAxis;
}

FVector ULiquidMovementComponent::GetCurrentRightAxis() const
{
	return CurrentRightAxis;
}

FVector ULiquidMovementComponent::GetGravityAtPlayerPosition() const
{
	return GravityAtPlayerPosition;
}

#pragma endregion 

#pragma region STATE TAGS

void ULiquidMovementComponent::EnableGrindingTag() const
{
	if (TimeSinceLastGrind < SecondsUntilWeCanGrind)
	{
		return;
	}
	LiquidPlayer->GameplayTags.AddTag(GrindingTag);
}

void ULiquidMovementComponent::DisableGrindingTag() const
{
	LiquidPlayer->GameplayTags.RemoveTag(GrindingTag);
}

void ULiquidMovementComponent::EnableDodgingTag()
{
	if (TimeSinceLastDodge < SecondsUntilWeCanDodge)
	{
		return;
	}
	TimeSinceLastDodge = 0;
	LiquidPlayer->GameplayTags.AddTag(DodgingTag);
}

void ULiquidMovementComponent::DisableDodgingTag() const
{
	LiquidPlayer->GameplayTags.RemoveTag(DodgingTag);
}

void ULiquidMovementComponent::AddWallRidingTag() const
{
	LiquidPlayer->GameplayTags.AddTag(WallRidingTag);
}

void ULiquidMovementComponent::RemoveWallRidingTag() const
{
	LiquidPlayer->GameplayTags.RemoveTag(WallRidingTag);
}

void ULiquidMovementComponent::AddBouncePadTag() const
{
	LiquidPlayer->GameplayTags.AddTag(BouncePadTag);
}

void ULiquidMovementComponent::RemoveBouncePadTag() const
{
	LiquidPlayer->GameplayTags.RemoveTag(BouncePadTag);
}

void ULiquidMovementComponent::AddJumpTag()
{
	if (TimeSinceLastJump <= SecondsUntilWeCanJump)
		return;
	LiquidPlayer->GameplayTags.AddTag(JumpingTag);
	TimeSinceLastJump = 0;
}

void ULiquidMovementComponent::RemoveJumpTag() const
{
	LiquidPlayer->GameplayTags.RemoveTag(JumpingTag);
}

bool ULiquidMovementComponent::HasJumpTag() const
{
	return LiquidPlayer->GameplayTags.HasTag(JumpingTag);
}

bool ULiquidMovementComponent::HasChangeGravityTag() const
{
	return LiquidPlayer->GameplayTags.HasTag(ChangeGravityTag);
}


#pragma endregion

void ULiquidMovementComponent::StartTaskTick()
{
	UpdateState();
	ClearContacts();
	UpdateContacts();
}

void ULiquidMovementComponent::ApplyTaskTick(float DeltaTime, float gravityMultiplier, bool overrideAcceleration, float overriddenAcceleration)
{
	// TODO: Fix this
	// Prevent player from flinging of when alt tabbing from the game
	if (DeltaTime > 0.1f || UGameplayStatics::IsGamePaused(GetWorld()))
	{
		return;
	}
	UpdateVelocity(DeltaTime, overrideAcceleration, overriddenAcceleration);
	
	//Apply Gravity

	if (!GetLockGravity() || LiquidGameInstance->Gravity.GetIsForcedGravity())
	{
		LiquidGameInstance->Gravity.GetGravity(GetPlayerFootLocation(),GravityAtPlayerPosition,  CurrentUpAxis);
	}
	else
	{
		GravityAtPlayerPosition = LockedGravity;
	}

	if (IsSimulatingPhysics())
	{
		if (IsGrounded())
		{
			PlayerCapsule->AddForce(GravityAtPlayerPosition * gravityMultiplier
				* 0.5f, NAME_None, true);
		}
		else
		{
			PlayerCapsule->AddForce(GravityAtPlayerPosition * gravityMultiplier
				, NAME_None, true);
		}
	}

	//Get the gravity alignment
	if (!GetLockGravity() || LiquidGameInstance->Gravity.GetIsForcedGravity())
	{
		GravityAlignment = LiquidGameInstance->Gravity.GetGravityAlignment(LiquidPlayer->GetActorLocation(), GravityAlignment);
	}
	else
	{
		GravityAlignment = LockedGravityOrientation;
	}

	CalculateAcceleration(DeltaTime);

	//Apply the calculated Acceleration if needed
	if (IsSimulatingPhysics())
	{
		PlayerCapsule->AddForce(DeltaAcceleration, NAME_None, true);
	}

	IncreaseTicksSinceLastJump();
	ClearState();
}

void ULiquidMovementComponent::SetTaskSmoothLocation(const FVector& targetLocation, float deltaTime, float positionTime,
                                                     float precision) const
{
	LiquidPlayer->SetActorLocation(
		FLiquidMath::LerpSmooth(LiquidPlayer->GetActorLocation(), targetLocation, deltaTime, positionTime, 0.01f)
		, false, nullptr, ETeleportType::TeleportPhysics);
}

void ULiquidMovementComponent::CalculateAcceleration(const float DeltaTime)
{
	if (!PlayerCapsule->IsSimulatingPhysics())
		return;

	// Calculate the change in velocity
	FVector DeltaVelocity = GetVelocity() - PlayerCapsule->GetPhysicsLinearVelocity();

	// Calculate the force to apply
	DeltaAcceleration = (DeltaVelocity / DeltaTime);
}

void ULiquidMovementComponent::UpdatePlayerInputSpaceAxes()
{
	const FTransform cameraSpace = LiquidPlayer->CameraHandler->LiquidCamera->GetTransform();
	const FVector cameraSpaceRight = cameraSpace.GetUnitAxis(EAxis::Y);
	const FVector cameraSpaceForward = cameraSpace.GetUnitAxis(EAxis::X);
	
	//Take Camera View into account to get the right and forward axis
	SetCurrentRightAxis(FLiquidMath::ProjectOnContactPlane(cameraSpaceRight, GetCurrentUpAxis()).GetSafeNormal());
	SetCurrentForwardAxis(FLiquidMath::ProjectOnContactPlane(cameraSpaceForward, GetCurrentUpAxis()).GetSafeNormal());
	//Up Axis is set by Gravity
}

void ULiquidMovementComponent::UpdateState()
{
	UpdatePlayerInputSpaceAxes();

	++StepsSinceLastGrounded;
	++StepsSinceLastJump;

	//TODO: Calculate in world space
	CurrentVelocity = PlayerCapsule->GetPhysicsLinearVelocity();
	
	//if (GetLockGravity() || LiquidGameInstance->Gravity.GetIsForcedGravity())
	//{
		//GEngine->AddOnScreenDebugMessage(-5,0.1f, FColor::Red,TEXT("Gravity Locked"));
	//}

	if (IsGrounded() || GetSnapToGround() || CheckSteepContact())
	{
		StepsSinceLastGrounded = 0;

		if (GroundContactCount > 1)
		{
			ContactNormal.Normalize();
		}
	}
	else
	{
		ContactNormal = CurrentUpAxis;
	}
}


void ULiquidMovementComponent::EvaluateCollisionNormal(const FVector& NormalVector)
{
	const float upDot = CurrentUpAxis.Dot(NormalVector);

	//If on ground
	if (upDot >= MinGroundDotProduct)
	{
		GroundContactCount += 1;
		ContactNormal += NormalVector;
	}

	//If on wall
	else if (upDot <= WallDotProductMinMax && upDot >= -WallDotProductMinMax)
	{
		WallContactCount += 1;
		WallNormal += NormalVector;
	}

	//If on steep
	else if (upDot > -0.01f)
	{
		SteepContactCount += 1;
		SteepNormal += NormalVector;
	}
}

void ULiquidMovementComponent::ClearState()
{
	InputMovementVector = FVector2D::ZeroVector;
	DesiredVelocity = FVector::ZeroVector;

	DeltaAcceleration = FVector::ZeroVector;
}

float ULiquidMovementComponent::GetAcceleration() const
{
	if (LiquidPlayer->GameplayTags.HasTag(WallRidingTag))
	{
		return MaxDefaultAcceleration;
	}
	if (IsGrounded())
	{
		if (InputMovementVector.Length() > 0.1f)
		{
			if (LiquidPlayer->GetVelocity().Length() > MaxDefaultSpeed)
			{
				return FMath::LerpStable(MaxDefaultAcceleration,MaxHighSpeedAcceleration,
					FMath::Clamp(1-(MaxHighSpeedSpeed-GetVelocity().Length())/(MaxHighSpeedSpeed-MaxDefaultSpeed)
					,0,1));
			}
			return MaxDefaultAcceleration;
		}
		return MaxDeceleration;
	}
	return MaxAirAcceleration;
}

void ULiquidMovementComponent::UpdateVelocity(float DeltaTime,  bool overrideAcceleration, float overriddenAcceleration)
{
	if (!bIsDesiredVelocityWorldSpace)
	{
		FVector xAxis, yAxis, zAxis;
		GetPlayerInputSpaceAxes(xAxis,yAxis,zAxis);
		
		float currentX = FVector::DotProduct(CurrentVelocity, xAxis);
		float currentY = FVector::DotProduct(CurrentVelocity, yAxis);
		float acceleration = GetAcceleration();
		
		if (overrideAcceleration)
		{
			acceleration = overriddenAcceleration;
		}

		float maxSpeedChange = acceleration * DeltaTime;


		float newX = FMath::FInterpTo(currentX, DesiredVelocity.X, DeltaTime, maxSpeedChange);
		float newY = FMath::FInterpTo(currentY, DesiredVelocity.Y, DeltaTime, maxSpeedChange);

		CurrentVelocity += xAxis * (newX - currentX) + yAxis * (newY - currentY);
	}
	else
	{
		FVector xAxis = FVector(1,0,0), yAxis = FVector(0,1,0);
		float currentX = FVector::DotProduct(CurrentVelocity, xAxis);
		float currentY = FVector::DotProduct(CurrentVelocity, yAxis);
		float acceleration = GetAcceleration();
		
		if (overrideAcceleration)
		{
			acceleration = overriddenAcceleration;
		}

		float maxSpeedChange = acceleration * DeltaTime;


		float newX = FMath::FInterpTo(currentX, DesiredVelocity.X, DeltaTime, maxSpeedChange);
		float newY = FMath::FInterpTo(currentY, DesiredVelocity.Y, DeltaTime, maxSpeedChange);

		CurrentVelocity += xAxis * (newX - currentX) + yAxis * (newY - currentY);	}
}


bool ULiquidMovementComponent::CheckSteepContact()
{
	if (SteepContactCount > 1)
	{
		SteepNormal.Normalize();

		float upDot = CurrentUpAxis.Dot(SteepNormal);
		if (upDot >= MinGroundDotProduct)
		{
			GroundContactCount = 1;
			ContactNormal = SteepNormal;
			return true;
		}
	}
	return false;
}

void ULiquidMovementComponent::RotateToLookDirection(float DeltaTime, float TimeToSmoothRotate, float RotatePrecision)
{
	FRotator newRotator = UKismetMathLibrary::MakeRotFromZX(GetCurrentUpAxis(), LookDirection);
	FQuat newQuat = UKismetMathLibrary::Conv_RotatorToQuaternion(newRotator);
	FQuat oldQuat = LiquidPlayer->GetActorRotation().Quaternion();

	FQuat interpolatedQuat = FLiquidMath::SlerpSmooth(oldQuat, newQuat, DeltaTime, TimeToSmoothRotate, RotatePrecision);
	
	ApplyRotation(interpolatedQuat.Rotator());
}

void ULiquidMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                             FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (LiquidPlayer->CapsuleComponent->GetCollisionEnabled() == ECollisionEnabled::Type::QueryOnly)
	{
		LiquidPlayer->CapsuleComponent->SetSimulatePhysics(false);
	}
	else
	{
		LiquidPlayer->CapsuleComponent->SetSimulatePhysics(true);
	}
	
	TimeSinceLastJump += DeltaTime;
	if (!IsGrinding())
	{
		TimeSinceLastGrind += DeltaTime;
	}
	if (!IsDodging())
	{
		TimeSinceLastDodge += DeltaTime;
	}

	LiquidPlayer->SetActorRotation(RotationToApply);
}

void ULiquidMovementComponent::ClearContacts()
{
	GroundContactCount = 0;
	SteepContactCount = 0;
	WallContactCount = 0;
	ContactNormal = FVector::ZeroVector;
	SteepNormal = FVector::ZeroVector;
	WallNormal = FVector::ZeroVector;
}

void ULiquidMovementComponent::UpdateContacts()
{
	TArray<AActor*> overlappingActors;
	LiquidPlayer->DefaultOverlappingSphere->GetOverlappingActors(overlappingActors);

	for (AActor* actor : overlappingActors)
	{
		if (actor->IsA<ALiquidPlayer>())
		{
			continue;
		}
		if(actor->IsA<ABaseEnemy>() || actor->IsA<AThrowableActor>())
		{
			continue;;
		}

		//Check if the actor has a mesh
		TArray<UStaticMeshComponent*> meshes;
		actor->GetComponents<UStaticMeshComponent>(meshes);

		for (auto &mesh: meshes)
		{
			FVector closestPoint{};
			mesh->GetClosestPointOnCollision(GetPlayerFootLocation()
				, closestPoint);

			//Not overlapping if it's far away, obviously.
			if((GetPlayerFootLocation()-closestPoint).Length() > LiquidPlayer->DefaultOverlappingSphere->GetScaledSphereRadius()*2)
			{
				continue;
			}
			
			FVector normalVector = (GetPlayerFootLocation() - closestPoint).GetSafeNormal();
			
			EvaluateCollisionNormal(normalVector);
		}
	}
}

void ULiquidMovementComponent::TryGrindRail(UGrindSplineComponent* GrindSplineComponent)
{
	if (IsStunting())
		return;
	if(TimeSinceLastGrind > SecondsUntilWeCanGrind)
	{
		CurrentRail = GrindSplineComponent;
		LiquidPlayer->GameplayTags.AddTag(GrindingTag);
	}
}