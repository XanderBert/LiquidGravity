#include "BaseTask.h"

#include "Kismet/GameplayStatics.h"
#include "LiquidGravity_Dev/Components/PlayerComponents/PlayerFeedbackComponent.h"
#include "LiquidGravity_Dev/Core/LiquidGameInstance.h"
#include "LiquidGravity_Dev/Core/LiquidPlayer.h"

EStateTreeRunStatus UBaseTask::EnterState(FStateTreeExecutionContext& Context,
                                          const FStateTreeTransitionResult& Transition)
{
 	Super::EnterState(Context, Transition);

 	if (OnEnterForceFeedback)
 	{
 		ALiquidPlayer* liquidPlayer = Cast<ALiquidPlayer>(GetWorld()->GetFirstPlayerController()->GetPawn());
 		if (IsValid(liquidPlayer))
 		{
 			liquidPlayer->FeedbackComponent->PlayForceFeedback(OnEnterForceFeedback);
 		}
 	}

 	if(OnEnterCameraShake.Get())
 	{
 		//Play camera shake
 		ALiquidPlayer* liquidPlayer = Cast<ALiquidPlayer>(GetWorld()->GetFirstPlayerController()->GetPawn());
 		if (IsValid(liquidPlayer))
 		{
 			liquidPlayer->FeedbackComponent->PlayScreenShake(OnEnterCameraShake);
 		}
 	}
 	
 	return EStateTreeRunStatus::Running;
 }
