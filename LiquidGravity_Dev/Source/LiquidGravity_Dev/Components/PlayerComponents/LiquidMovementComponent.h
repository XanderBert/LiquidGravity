#pragma once

#include "CoreMinimal.h"
#include "../GrindSplineComponent.h"
#include "Components/ActorComponent.h"
#include "Components/CapsuleComponent.h"
#include "LiquidGravity_Dev/Core/LiquidPlayer.h"
#include "LiquidGravity_Dev/PlayerStates/JumpPadTask.h"
#include "LiquidMovementComponent.generated.h"


class UBouncePadComponent;
class UCapsuleComponent;
class ULiquidGameInstance;
class ALiquidPlayer;
struct FInputActionValue;

UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LIQUIDGRAVITY_DEV_API ULiquidMovementComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	ULiquidMovementComponent();

	FVector LookDirection = FVector(1,0,0);
	void RotateToLookDirection(float DeltaTime, float TimeToSmoothRotate, float RotatePrecision);

	bool IsSimulatingPhysics () const { return LiquidPlayer->CapsuleComponent->IsSimulatingPhysics(); }
	bool bGrindDirectionChangeIntended = false;

	//-------------------- Task Functions --------------------
	void StartTaskTick();
	void ApplyTaskTick(float DeltaTime, float gravityMultiplier = 1.f,bool overrideAcceleration = false, float overriddenAcceleration = 1.f);

	void SetTaskSmoothLocation(const FVector& targetLocation, float deltaTime, float positionTime, float precision) const;
	void SetTaskSmoothRotation(const FQuat& targetRotation, float deltaTime, float rotationTime, float precision);

	//-------------------- Input Functions --------------------
	void HandleMovementInput(const FInputActionValue& Value);
	
	void HandleInteractInput();

	// For changing velocity from a task
	void AddTaskVelocityChange(const FVector& velocityChange, bool isWorldSpace = true);
	// For setting velocity directly -> Eg: Jumping
	void SetTaskVelocity(const FVector& velocityChange, float deltaTime = 0, bool isWorldSpace = true);

	//------------------ Time Logics ----------------------
	float TimeSinceLastGrind{};
	float TimeSinceLastDodge{};
	float TimeSinceLastJump{};

	float GetAcceleration() const;

	FVector OverridenGrindingVelocity;
	
	//--------------------  Jump logic  --------------------
	UFUNCTION(BlueprintGetter)
	int GetMaxAirJumps() const;

	UFUNCTION(BlueprintCallable)
	void ResetTicksSinceLastJump();

	UFUNCTION(BlueprintCallable)
	void IncreaseTicksSinceLastJump();

	UFUNCTION(BlueprintGetter)
	float GetJumpHeight() const;
	
	UFUNCTION(BlueprintGetter)
	float GetGravityStrength() const;

	UFUNCTION(BlueprintGetter)
	bool GetJumpingFromSpline()
	const
	{
		return bJumpingFromSpline;
	};

	UFUNCTION(BlueprintSetter)
	void SetJumpingFromSpline(bool value);
	
	UFUNCTION(BlueprintGetter)
	UGrindSplineComponent* GetCurrentRail() const;

	//--------------------- Jumping logic ------------------
	void AddJumpTag();
	void RemoveJumpTag() const;
	bool HasJumpTag() const;
	UFUNCTION(BlueprintPure)
	bool HasChangeGravityTag() const;

	//--------------------  Grinding logic  --------------------
	UFUNCTION(BlueprintPure)
	bool IsGrinding() const;

	void EnableGrindingTag() const;

	void DisableGrindingTag() const;

	//--------------------  Dodging logic  --------------------
	void EnableDodgingTag();
	
	void DisableDodgingTag() const;
	
	
	UFUNCTION(BlueprintGetter, BlueprintCallable)
	bool IsDodging();

	
	//--------------------  Wall Riding logic  --------------------
	UFUNCTION(BlueprintPure)
	bool IsWallRiding();

	void AddWallRidingTag() const;

	void RemoveWallRidingTag() const;

	//-------------------- BouncePad Logic -----------------------
	void AddBouncePadTag() const;
	void RemoveBouncePadTag() const;


	//-------------------- Getters and Setters --------------------

	void GetPlayerInputSpaceAxes(FVector &XAxis, FVector &YAxis, FVector &ZAxis) const;
	
	UFUNCTION(BlueprintCallable)
	void SetBouncePad(UBouncePadComponent* UBouncePad);

	UFUNCTION(BlueprintCallable)
	UBouncePadComponent* GetBouncePad();

	UFUNCTION(BlueprintPure)
	bool IsGrounded() const;
	
	UFUNCTION(BlueprintPure)
	bool IsOnWall() const;

	UFUNCTION(BlueprintPure)
	bool OnSteepContact() const;

	UFUNCTION(BlueprintPure)
	float GetMaxSpeed() const;

	UFUNCTION(BlueprintGetter, BlueprintCallable)
	FVector GetVelocity() const;
	
	UFUNCTION(BlueprintPure)
	FVector GetDisplayVelocity() const;

	UFUNCTION(BlueprintPure)
	FVector GetContactNormal() const;

	UFUNCTION(BlueprintPure)
	FVector GetWallNormal() const;

	UFUNCTION(BlueprintGetter)
	FVector GetSteepNormal() const;

	UFUNCTION(BlueprintGetter)
	FVector GetCurrentForwardAxis() const { return CurrentForwardAxis; }

	UFUNCTION(BlueprintSetter)
	void SetCurrentForwardAxis(const FVector& newForwardAxis) { CurrentForwardAxis = newForwardAxis; }

	UFUNCTION(BlueprintGetter)
	FVector GetCurrentUpAxis() const;

	UFUNCTION(BlueprintSetter)
	void SetCurrentUpAxis(const FVector& newUpAxis) { CurrentUpAxis = newUpAxis; }

	UFUNCTION(BlueprintGetter)
	FVector GetCurrentRightAxis() const;

	UFUNCTION(BlueprintSetter)
	void SetCurrentRightAxis(const FVector& newRightAxis) { CurrentRightAxis = newRightAxis; }

	UFUNCTION(BlueprintPure)
	FVector GetGravityAtPlayerPosition() const;

	UFUNCTION(BlueprintCallable)
	void ApplyRotation(const FRotator& rotation);

	UFUNCTION(BlueprintGetter)
	FVector2D GetInputMovementVector() const;

	UFUNCTION(BlueprintSetter)
	//For Setting Velocity that player should reach for eg: while skating
	void SetDesiredVelocity(const FVector& newDesiredVelocity, bool isWorldSpace = false);

	UFUNCTION(BlueprintGetter, BlueprintCallable)
	FQuat GetGravityAlignment() const;

	UFUNCTION(BlueprintSetter, BlueprintCallable)
	void SetGravityAlignment(const FQuat& newGravityAlignment);

	UFUNCTION(BlueprintGetter)
	float GetActorRotationSpeed() const;

	UFUNCTION(BlueprintSetter)
	void SetSnapToGround(bool value);

	UFUNCTION(BlueprintGetter)
	bool IsVelocityWorldSpace() const;

	UFUNCTION(BlueprintSetter)
	void SetVelocityWorldSpace(bool value);

	bool IsStunting() const;
	void TryGrindRail(UGrindSplineComponent* GrindSplineComponent);

	void SetLockGravity(bool bNewLockGravity, bool bForceReset = false);
	void SetForcedGravity(const FVector& newGravity);

	UFUNCTION(BlueprintPure)
	bool GetLockGravity() const;

	UFUNCTION(BlueprintGetter)
	FQuat GetLockGravityOrientation() const;


	UPROPERTY()
	UGrindSplineComponent* CurrentRail = nullptr;

	void LockGravityFlipFlop();
	
protected:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere)
	float FootDisplacement = 50;
	
	//--------- Gameplay Tags ------
	UPROPERTY(EditAnywhere)
	FGameplayTag SkatingTag;
	UPROPERTY(EditAnywhere)
	FGameplayTag GrindingTag;
	UPROPERTY(EditAnywhere)
	FGameplayTag JumpingTag;
	UPROPERTY(EditAnywhere)
	FGameplayTag BouncePadTag;
	UPROPERTY(EditAnywhere)
	FGameplayTag WallRidingTag;
	UPROPERTY(EditAnywhere)
	FGameplayTag DodgingTag;
	UPROPERTY(EditAnywhere)
	FGameplayTag HookingTag;
	UPROPERTY(EditAnywhere)
	FGameplayTag ChangeGravityTag;
	
	void CalculateAcceleration(const float DeltaTime);
	void EvaluateCollisionNormal(const FVector& Vector);
	void UpdateState();
	void ClearState();
	void UpdateVelocity(float DeltaTime, bool overrideAcceleration = false, float overriddenAcceleration = 0.1f);
	bool GetSnapToGround();
	void UpdatePlayerInputSpaceAxes();
	bool CheckSteepContact();
	void ClearContacts();
	FVector GetPlayerFootLocation() const;
	void UpdateContacts();
	bool bLockGravity = false;
	FVector LockedGravity;
	FQuat LockedGravityOrientation;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float MaxDefaultSpeed = 500.0f;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float MaxHighSpeedSpeed = 1000.0f;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float MaxSnapSpeed = 400.0f;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float MaxDefaultAcceleration = 10.0f;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float MaxHighSpeedAcceleration = 5.0f;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float MaxAirAcceleration = .3f;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float MaxDeceleration = .1f;
	
	UPROPERTY(EditAnywhere, Category = "Movement")
	float JumpHeight = 0.04f;

	UPROPERTY(EditAnywhere, Category = "Movement")
	int MaxAirJumps = 4;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float MaxGroundAngle = 45.0f;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float WallAngleDelta = 15.0f;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float ActorRotationSpeed = 30.0f;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float SecondsUntilWeCanJump = 0.5f;
	
	UPROPERTY(EditAnywhere, Category = "Movement")
	float SecondsUntilWeCanGrind = 0.5f;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float SecondsUntilWeCanDodge = 2.0f;

	UPROPERTY(EditAnywhere, Category = "Movement")
	bool bSnapToGround{false};
	
	FVector2D InputMovementVector{};
	FVector CurrentVelocity{};
	FVector DesiredVelocity{};
	FVector ContactNormal{};
	FVector SteepNormal{};
	FVector WallNormal{};

	FVector CurrentUpAxis{};
	FVector CurrentRightAxis{};
	FVector CurrentForwardAxis{};

	FVector DeltaAcceleration{};
	FRotator RotationToApply{};

	FVector GravityAtPlayerPosition{};

	FQuat GravityAlignment{FQuat::Identity};

	int GroundContactCount{};
	int SteepContactCount{};
	int WallContactCount{};
	int StepsSinceLastGrounded{};
	int StepsSinceLastJump{};
	float MinGroundDotProduct{};
	float WallDotProductMinMax{};
	bool bIsDesiredVelocityWorldSpace{false};
	bool bJumpingFromSpline{};

	UPROPERTY()
	UBouncePadComponent* BouncePad{};
	
	UPROPERTY()
	UCapsuleComponent* PlayerCapsule{};

	UPROPERTY()
	ALiquidPlayer* LiquidPlayer{};

	UPROPERTY()
	ULiquidGameInstance* LiquidGameInstance{};
};
