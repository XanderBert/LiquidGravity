#pragma once

#include "CoreMinimal.h"
#include "Blueprint/StateTreeTaskBlueprintBase.h"
#include "BaseTask.generated.h"

 UCLASS()
 class LIQUIDGRAVITY_DEV_API UBaseTask : public UStateTreeTaskBlueprintBase
 {
 	GENERATED_BODY()

 protected:
 	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) override;
 	
 	UPROPERTY(EditAnywhere, Category = Parameter)
 	UForceFeedbackEffect* OnEnterForceFeedback;

 	UPROPERTY(EditAnywhere, Category = Parameter)
 	TSubclassOf<UCameraShakeBase> OnEnterCameraShake;

 };