#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerFeedbackComponent.generated.h"

enum class UStuntEnum : uint8;
class ALiquidPlayer;
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FFeedbackSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FFeedbackSignatureFloat, float, strength);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FFeedbackSignatureEnum, UStuntEnum, stuntEnum);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FFeedbackSignatureTwoVectorParams, FVector, startPosition, FVector, endPosition);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FFeedbackSignatureBool, bool, isUsingController);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LIQUIDGRAVITY_DEV_API UPlayerFeedbackComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPlayerFeedbackComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(BlueprintAssignable, Category = "Player Feedback")
	FFeedbackSignature OnJump;
	UPROPERTY(BlueprintAssignable, Category = "Player Feedback")
	FFeedbackSignature OnAirJump;
	UPROPERTY(BlueprintAssignable, Category = "Player Feedback")
	FFeedbackSignatureFloat OnLand; 
	UPROPERTY(BlueprintAssignable, Category = "Player Feedback")
	FFeedbackSignature OnGrind;
	UPROPERTY(BlueprintAssignable, Category = "Player Feedback")
	FFeedbackSignature OnSkateStarted;
	UPROPERTY(BlueprintAssignable, Category = "Player Feedback")
	FFeedbackSignature OnHook;
	UPROPERTY(BlueprintAssignable, Category = "Player Feedback")
	FFeedbackSignature OnDodge;
	UPROPERTY(BlueprintAssignable, Category = "Player Feedback")
	FFeedbackSignature OnChangeGravity;
	UPROPERTY(BlueprintAssignable, Category = "Player Feedback")
	FFeedbackSignature OnJumpPad;
	UPROPERTY(BlueprintAssignable, Category = "Player Feedback")
	FFeedbackSignature OnGrindEnded;
	UPROPERTY(BlueprintAssignable, Category = "Player Feedback")
	FFeedbackSignatureTwoVectorParams OnShoot;
	UPROPERTY(BlueprintAssignable, Category = "Player Feedback")
	FFeedbackSignature OnLockOn;
	UPROPERTY(BlueprintAssignable, Category = "Player Feedback")
	FFeedbackSignature OnStuntAdded;
	UPROPERTY(BlueprintAssignable, Category = "Player Feedback")
	FFeedbackSignature OnStuntSuccessful;
	UPROPERTY(BlueprintAssignable, Category = "Player Feedback")
	FFeedbackSignature OnStuntFailed;
	UPROPERTY(BlueprintAssignable, Category = "Player Feedback")
	FFeedbackSignature OnCombatObjectPulled;
	UPROPERTY(BlueprintAssignable, Category = "Player Feedback")
	FFeedbackSignature OnCombatObjectPushed;
	UPROPERTY(BlueprintAssignable, Category = "Player Feedback")
	FFeedbackSignature OnKick;
	UPROPERTY(BlueprintAssignable, Category = "Player Feedback")
	FFeedbackSignatureEnum OnStuntStarted;
	UPROPERTY(BlueprintAssignable, Category = "Player Feedback")
	FFeedbackSignature OnEnemyKilled;
	UPROPERTY(BlueprintAssignable, Category = "Player Feedback")
	FFeedbackSignature OnGrindDirectionChange;
	UPROPERTY(BlueprintAssignable, Category = "Player Feedback")
	FFeedbackSignatureBool OnControllerChanged;

	UFUNCTION(BlueprintCallable)
	void PlaySound(USoundBase* sound);

	UFUNCTION(BlueprintCallable)
	void PlaySoundRandomPitch(USoundBase* sound, float minPitch = 0.8f, float maxPitch = 1.2f);
	UFUNCTION(BlueprintCallable)
	void PlayForceFeedback(UForceFeedbackEffect* forceFeedback) const;
	UFUNCTION(BlueprintCallable)
	void PlayScreenShake(TSubclassOf<UCameraShakeBase> cameraShake) const;
	

private:
	UPROPERTY()
	ALiquidPlayer* LiquidPlayer;

};
