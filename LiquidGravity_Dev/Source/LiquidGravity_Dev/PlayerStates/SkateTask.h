#pragma once

#include "CoreMinimal.h"
#include "BaseTask.h"

#include "StateTreeTaskBase.h"
#include "Blueprint/StateTreeTaskBlueprintBase.h"

#include "SkateTask.generated.h"


class ALiquidCamera;
class ALiquidPlayer;
class ULiquidMovementComponent;
class UCapsuleComponent;

UCLASS(meta = (DisplayName = "STT_Skate"))
class LIQUIDGRAVITY_DEV_API USkateTask : public UBaseTask 
{
	GENERATED_BODY()

	void OnLandEffects() const;
	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) override;
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Parameter, meta = (AllowPrivateAccess = "true"))
	ULiquidMovementComponent* MovementComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Parameter, meta = (AllowPrivateAccess = "true"))
	ALiquidPlayer* LiquidPlayer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Parameter, meta = (AllowPrivateAccess = "true"))
	ALiquidCamera* LiquidCamera;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Parameter, meta = (AllowPrivateAccess = "true"))
	UDataTable* EnergyAdditionTable;

	float TimeSinceEnergyAddition = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Parameter, meta = (AllowPrivateAccess = "true"))
	float TimeToSmoothRotate = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Parameter, meta = (AllowPrivateAccess = "true"))
	float RotatePrecision = 0.01f;

	UPROPERTY(EditAnywhere, Category = Parameter)
	UForceFeedbackEffect* OnLandForceFeedback;

	UPROPERTY(EditAnywhere, Category = Parameter)
	TSubclassOf<UCameraShakeBase> OnLandCameraShake;

	bool isGroundedTracker = false;
	FVector previousFrameVelocity;
};