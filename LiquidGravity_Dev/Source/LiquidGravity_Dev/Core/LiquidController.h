#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "LiquidGravity_Dev/Stunts/UStuntEnum.h"
#include "LiquidController.generated.h"

class ALiquidHUD;
enum class UStuntEnum : uint8;
struct FFlickInputSettings;
struct FInputActionValue;
class ALiquidPlayer;
class UInputAction;
class UInputMappingContext;
class ULiquidMovementComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTrailerSkippedSignature);

UCLASS()
class LIQUIDGRAVITY_DEV_API ALiquidController : public APlayerController
{
	GENERATED_BODY()
public:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	void StuntButtonHeld();
	void StuntButtonReleased();
	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION(BlueprintPure)
	bool IsPlayingWithController() const;
	
	UFUNCTION(BlueprintCallable)
	void SwitchToMainMenuInput();

	UFUNCTION(BlueprintCallable)
	void SwitchToGameInput();
	
	UFUNCTION(BlueprintCallable)
	void SwitchToPauseInput();

	UFUNCTION(BlueprintCallable)
	void SwitchToTrailerInput();

	UFUNCTION()
	void TrailerFinished();
	
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FOnTrailerSkippedSignature OnTrailerSkippedOrEnded;
private:
	UFUNCTION()
	void OnTrailerStartSkip();
	
	UFUNCTION()
	void OnTrailerEndSkip();

	UFUNCTION()
	void OnWidgetSkippedFilled();

	UPROPERTY()
	ALiquidHUD* LiquidHUD{}; 
	
	UFUNCTION()
	void OnInputDeviceConnectionChange(const FPlatformUserId UserId, const FInputDeviceId DeviceId);

	
	
	//-------------------- Input  --------------------
	UPROPERTY(EditDefaultsOnly, Category = Input, meta = (DisplayName = "Default Mapping Context"))
	UInputMappingContext* MappingContext;

	UPROPERTY(EditDefaultsOnly, Category = Input, meta = (DisplayName = "Main Menu Mapping Context"))
	UInputMappingContext* MainMenuMappingContext;

	UPROPERTY(EditDefaultsOnly, Category = Input, meta = (DisplayName = "Pause Mapping Context"))
	UInputMappingContext* PauseMenuMappingContext;

	UPROPERTY(EditDefaultsOnly, Category = Input, meta = (DisplayName = "Trailer Mapping Context"))
	UInputMappingContext* TrailerMapping;
	
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* MoveAction;
	
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* LookAction;
	
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* JumpAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* ChangeGravityAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* InteractAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* FireAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* HookingAction;
	
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* FlickUpDownAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* FlickLeftRightAction;
	
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* DodgeAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* LockGravityAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* PickUpAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* LockCameraAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* PauseAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* StuntModifier;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* SkipTrailer;
	
	//-------------------- Input  Functions --------------------
	float TimeStunButtonReleased = 0.f;
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void Dodge(const FInputActionValue& Value);
	void LockGravity(const FInputActionValue& Value);
	void Jump();
	void Interact();
	void LockCamera();
	void ChangeGravity();
	void StartShooting();
	void StopShooting();
	void TryHook();
	void PickUp();

	void ForwardHorizontalFlick(const FInputActionValue& Value);
	void ForwardVerticalFlick(const FInputActionValue& Value);

	void ForwardToMainMenu();
	void PauseMenu();

	bool IsStuntButtonHeld;
	bool IsUsingController = false;

	bool IsStuntInputValid();
	UFUNCTION()
	void StartStunt(UStuntEnum StuntEnum);
	
	UPROPERTY()
	ALiquidPlayer* LiquidPlayer;

	UPROPERTY()
	ULiquidMovementComponent* LiquidMovementComponent;
	
	UPROPERTY(EditAnywhere, Category = "Flick Input")
	TArray<FFlickInputSettings> VerticalFlickSettings{};

	UPROPERTY(EditAnywhere, Category = "Flick Input")
	TArray<FFlickInputSettings> HorizontalFlickSettings{};
};