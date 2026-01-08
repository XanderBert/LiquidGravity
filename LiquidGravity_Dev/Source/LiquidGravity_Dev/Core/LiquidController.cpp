#include "LiquidController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "LiquidCamera.h"
#include "LiquidHUD.h"
#include "LiquidPlayer.h"
#include "GameFramework/InputDeviceSubsystem.h"
#include "LiquidGravity_Dev/Components/PlayerComponents/CameraHandler.h"
#include "LiquidGravity_Dev/Components/PlayerComponents/ChangeGravityComponent.h"
#include "LiquidGravity_Dev/Components/PlayerComponents/InteractComponent.h"
#include "LiquidGravity_Dev/Components/PlayerComponents/LiquidMovementComponent.h"
#include "LiquidGravity_Dev/Components/PlayerComponents/PlayerHookingComponent.h"
#include "LiquidGravity_Dev/Components/PlayerComponents/CombatComponent.h"
#include "LiquidGravity_Dev/Components/PlayerComponents/PickupComponent.h"
#include "LiquidGravity_Dev/Components/PlayerComponents/PlayerFeedbackComponent.h"
#include "LiquidGravity_Dev/Components/PlayerComponents/PlayerStuntComponent.h"
#include "LiquidGravity_Dev/Stunts/FlickInputSettings.h"
#include "LiquidGravity_Dev/Widgets/HoldToSkipWidget.h"


void ALiquidController::BeginPlay()
{
	Super::BeginPlay();
	LiquidPlayer = Cast<ALiquidPlayer>(GetPawn());
	ensureMsgf(LiquidPlayer != nullptr, TEXT("AGraviController::BeginPlay -> No valid liquid player! Check log to see if it spawned Correctly"));
	LiquidMovementComponent = LiquidPlayer->LiquidMovementComponent;
	ensureMsgf(LiquidMovementComponent != nullptr, TEXT("AGraviController::BeginPlay -> No valid liquid movement component! Check log to see if it spawned Correctly"));

	LiquidHUD = Cast<ALiquidHUD>(GetHUD());
	ensureMsgf(LiquidHUD != nullptr, TEXT("AGraviController::BeginPlay -> No valid liquid HUD! Check log to see if it spawned Correctly"));

	LiquidHUD->OnSkipped.BindDynamic(this, &ALiquidController::OnWidgetSkippedFilled);
	LiquidHUD->OnDisableMenu.AddDynamic(this, &ALiquidController::SwitchToGameInput);
	LiquidHUD->OnPause.AddDynamic(this, &ALiquidController::SwitchToPauseInput);
	LiquidHUD->OnUnpause.AddDynamic(this, &ALiquidController::SwitchToGameInput);
	

	UInputDeviceSubsystem* InputDeviceSubsystem = GetWorld()->GetGameInstance()->GetEngine()->GetEngineSubsystem<UInputDeviceSubsystem>();
	InputDeviceSubsystem->OnInputHardwareDeviceChanged.AddDynamic(this, &ALiquidController::OnInputDeviceConnectionChange);
	
	OnTrailerSkippedOrEnded.AddDynamic(this, &ALiquidController::TrailerFinished);
}

void ALiquidController::SetupInputComponent()
{
	Super::SetupInputComponent();
	
	for(auto& flick : HorizontalFlickSettings)
	{
		flick.OnFlickPerformed.AddDynamic(this, &ALiquidController::StartStunt);
	}

	for(auto& flick : VerticalFlickSettings)
	{
		flick.OnFlickPerformed.AddDynamic(this, &ALiquidController::StartStunt);
	}
	
	UEnhancedInputLocalPlayerSubsystem* subSystem{ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer())};
	UEnhancedInputComponent* enhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);
	ensureMsgf(enhancedInputComponent != nullptr, TEXT("ABasePlayerCharacter::SetupPlayerInputComponent -> No valid enhanced input component"));
	ensureMsgf(subSystem != nullptr, TEXT("AGraviController::SetupInputComponent -> No valid enhanced input local sub system!"));
	ensureMsgf(MappingContext != nullptr, TEXT("AGraviController::SetupInputComponent -> No valid mapping context"));
	ensureMsgf(MainMenuMappingContext != nullptr, TEXT("AGraviController::SetupInputComponent -> No valid mapping context"));
	
	enhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ALiquidController::Move);
	enhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ALiquidController::ForwardToMainMenu);
	enhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ALiquidController::Look);
	enhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ALiquidController::Jump);
	enhancedInputComponent->BindAction(DodgeAction, ETriggerEvent::Started, this, &ALiquidController::Dodge);
	enhancedInputComponent->BindAction(ChangeGravityAction, ETriggerEvent::Started, this, &ALiquidController::ChangeGravity);
	enhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &ALiquidController::Interact);
	enhancedInputComponent->BindAction(FireAction, ETriggerEvent::Triggered, this, &ALiquidController::StartShooting);
	enhancedInputComponent->BindAction(FireAction, ETriggerEvent::Completed, this, &ALiquidController::StopShooting);
	enhancedInputComponent->BindAction(HookingAction, ETriggerEvent::Started, this, &ALiquidController::TryHook);
	enhancedInputComponent->BindAction(LockGravityAction, ETriggerEvent::Started, this, &ALiquidController::LockGravity);
	enhancedInputComponent->BindAction(PickUpAction, ETriggerEvent::Started, this, &ALiquidController::PickUp);
	enhancedInputComponent->BindAction(LockCameraAction, ETriggerEvent::Started, this, &ALiquidController::LockCamera);
	enhancedInputComponent->BindAction(PauseAction, ETriggerEvent::Started, this, &ALiquidController::PauseMenu);
	enhancedInputComponent->BindAction(StuntModifier, ETriggerEvent::Started, this, &ALiquidController::StuntButtonHeld);
	enhancedInputComponent->BindAction(StuntModifier, ETriggerEvent::Completed, this, &ALiquidController::StuntButtonReleased);

	
	enhancedInputComponent->BindAction(SkipTrailer, ETriggerEvent::Triggered, this, &ALiquidController::OnTrailerStartSkip);
	enhancedInputComponent->BindAction(SkipTrailer, ETriggerEvent::Completed, this, &ALiquidController::OnTrailerEndSkip);
	
	SwitchToTrailerInput();
}

void ALiquidController::StuntButtonHeld()
{
	IsStuntButtonHeld = true;
}

void ALiquidController::StuntButtonReleased()
{
	IsStuntButtonHeld = false;
	TimeStunButtonReleased = GetWorld()->GetTimeSeconds();
}

void ALiquidController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	for(auto& flick : HorizontalFlickSettings)
	{
		flick.Tick(DeltaSeconds);
	}

	for(auto& flick : VerticalFlickSettings)
	{
		flick.Tick(DeltaSeconds);
	}
}

bool ALiquidController::IsPlayingWithController() const
{
	return IsUsingController;
}

void ALiquidController::SwitchToMainMenuInput()
{
	UEnhancedInputLocalPlayerSubsystem* subSystem{ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer())};
	ensureMsgf(subSystem != nullptr, TEXT("AGraviController::SetupInputComponent -> No valid enhanced input local sub system!"));

	subSystem->ClearAllMappings();
	subSystem->AddMappingContext(MainMenuMappingContext, 0);
}

void ALiquidController::SwitchToGameInput()
{
	UEnhancedInputLocalPlayerSubsystem* subSystem{ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer())};
	ensureMsgf(subSystem != nullptr, TEXT("AGraviController::SetupInputComponent -> No valid enhanced input local sub system!"));

	subSystem->ClearAllMappings();
	subSystem->AddMappingContext(MappingContext, 0);
}

void ALiquidController::SwitchToPauseInput()
{
	bShowMouseCursor = true;
	
	UEnhancedInputLocalPlayerSubsystem* subSystem{ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer())};
	ensureMsgf(subSystem != nullptr, TEXT("AGraviController::SetupInputComponent -> No valid enhanced input local sub system!"));

	subSystem->ClearAllMappings();
	subSystem->AddMappingContext(PauseMenuMappingContext, 0);
}

void ALiquidController::SwitchToTrailerInput()
{
	UEnhancedInputLocalPlayerSubsystem* subSystem{ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer())};
	ensureMsgf(subSystem != nullptr, TEXT("AGraviController::SetupInputComponent -> No valid enhanced input local sub system!"));

	subSystem->ClearAllMappings();
	subSystem->AddMappingContext(TrailerMapping, 0);
}

void ALiquidController::TrailerFinished()
{
	LiquidHUD->SetupMainMenu();
	SwitchToMainMenuInput();
}

void ALiquidController::OnTrailerStartSkip()
{
	LiquidHUD->TrailerHUD->StartHoldToSkip();
}

void ALiquidController::OnTrailerEndSkip()
{
	LiquidHUD->TrailerHUD->StopHoldToSkip();
}

void ALiquidController::OnWidgetSkippedFilled()
{
	OnTrailerSkippedOrEnded.Broadcast();
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Silver, TEXT("Skipped"));
}

void ALiquidController::OnInputDeviceConnectionChange(const FPlatformUserId UserId, const FInputDeviceId DeviceId)
{
	// Get the Input Device Subsystem
	UInputDeviceSubsystem* InputDeviceSubsystem = GetWorld()->GetGameInstance()->GetEngine()->GetEngineSubsystem<UInputDeviceSubsystem>();
	if (!InputDeviceSubsystem) return;
	
	
	// Check if the device is connected
	FHardwareDeviceIdentifier RecentlyUsedHardware = InputDeviceSubsystem->GetMostRecentlyUsedHardwareDevice(UserId);
	EHardwareDevicePrimaryType hardwareType = RecentlyUsedHardware.PrimaryDeviceType;

	IsUsingController = hardwareType == EHardwareDevicePrimaryType::Gamepad;

	LiquidPlayer->FeedbackComponent->OnControllerChanged.Broadcast(IsUsingController);
}



void ALiquidController::Move(const FInputActionValue& Value)
{
	for(auto& flick : HorizontalFlickSettings)
	{
		flick.ForwardInput(Value.Get<FVector2D>().X);
	}

	for(auto& flick : VerticalFlickSettings)
	{
		flick.ForwardInput(Value.Get<FVector2D>().Y);
	}
	
	LiquidMovementComponent->HandleMovementInput(Value);
}

void ALiquidController::Look(const FInputActionValue& Value)
{
	LiquidPlayer->CameraHandler->LiquidCamera->RotateCamera(Value);	
}

void ALiquidController::Dodge(const FInputActionValue& Value)
{
	LiquidMovementComponent->EnableDodgingTag();
}

void ALiquidController::LockGravity(const FInputActionValue& Value)
{
	LiquidMovementComponent->LockGravityFlipFlop();
}

void ALiquidController::Jump()
{
	LiquidMovementComponent->AddJumpTag();
}

//TODO: Priorities
void ALiquidController::Interact()
{
	LiquidMovementComponent->HandleInteractInput();
	LiquidPlayer->InteractComponent->Interact();
}

void ALiquidController::LockCamera()
{
	//LiquidPlayer->LiquidCamera->ResetCamera();
	LiquidPlayer->CombatComponent->LockOn();
}


void ALiquidController::ChangeGravity()
{
	LiquidPlayer->ChangeGravityComponent->SetLookedAtGravityFieldActive();
}

void ALiquidController::StartShooting()
{
	LiquidPlayer->CombatComponent->EvaluateCombat();
}

void ALiquidController::StopShooting()
{
	LiquidPlayer->CombatComponent->EndEvaluateCombat();
}

void ALiquidController::TryHook()
{
	LiquidPlayer->HookingComponent->TryHook();
}

void ALiquidController::PickUp()
{
	LiquidPlayer->PickupComponent->Pickup();
}



void ALiquidController::ForwardToMainMenu()
{
	Cast<ALiquidHUD>(GetHUD())->OnStartMenuInputReceived();
}

void ALiquidController::PauseMenu()
{
	Cast<ALiquidHUD>(GetHUD())->PauseMenu();
}

bool ALiquidController::IsStuntInputValid()
{
	return IsStuntButtonHeld || FMath::Abs(GetWorld()->GetTimeSeconds() - TimeStunButtonReleased) < 1.f;
}

void ALiquidController::StartStunt(UStuntEnum StuntEnum)
{
	if (!IsStuntInputValid())
		return;
	
	//Reset all flicks
	for(auto& flick : HorizontalFlickSettings)
	{
		flick.Reset();
	}

	for(auto& flick : VerticalFlickSettings)
	{
		flick.Reset();
	}
	
	LiquidPlayer->StuntComponent->StartStunt(StuntEnum);
}
