#include "LiquidHUD.h"

#include "LiquidGameInstance.h"
#include "LiquidPlayer.h"
#include "Blueprint/UserWidget.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "LiquidGravity_Dev/Widgets/HoldToSkipWidget.h"

void ALiquidHUD::BeginPlay()
{
	Super::BeginPlay();

	
	SetupTrailerHUD();
}

void ALiquidHUD::SetupTrailerHUD()
{
	check(TrailerHUDClass);
	
	// Create the HUD
	TrailerHUD = CreateWidget<UHoldToSkipWidget>(GetWorld(), TrailerHUDClass);
	if(TrailerHUD)
	{
		TrailerHUD->AddToViewport();
		TrailerHUD->SetVisibility(ESlateVisibility::Visible);
	}
	
}

void ALiquidHUD::SetupMainMenu()
{
	RemoveWidgetFromViewport(TrailerHUD);
	
	bIsMainMenuActive = true;
	MainMenuHUD = AddWidgetToViewport(MainMenuHUDClass);
}

void ALiquidHUD::OnStartMenuInputReceived()
{
	if(bIsMainMenuActive)
	{
		RemoveWidgetFromViewport(MainMenuHUD);
		AddPlayerHUD();

		bIsMainMenuActive = false;
		OnDisableMenu.Broadcast();
	}
}

void ALiquidHUD::PauseMenu()
{
	//Create the pause menu
	PauseMenuWidget = AddWidgetToViewport(PauseMenuClass);
	OnPause.Broadcast();
}

void ALiquidHUD::UnpauseMenu()
{
	//Hide the mouse cursor and set focus back to the game
	auto controller = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	check(controller);
	controller->SetShowMouseCursor(false);
	controller->SetInputMode(FInputModeGameOnly{});


	//Remove the pause menu
	RemoveWidgetFromViewport(PauseMenuWidget);
	OnUnpause.Broadcast();
}

void ALiquidHUD::RemovePlayerHUD()
{
	RemoveWidgetFromViewport(PlayerHUD);
}

void ALiquidHUD::AddPlayerHUD()
{
	PlayerHUD = AddWidgetToViewport(PlayerHUDClass);
}

UUserWidget* ALiquidHUD::ShowEndScreen()
{
	EndingScreen = AddWidgetToViewport(EndingScreenClass);
	return EndingScreen;
}

UUserWidget* ALiquidHUD::AddWidgetToViewport(TSubclassOf<class UUserWidget> widget) const
{
	check(widget);
	
	// Create the HUD
	UUserWidget* spawnedWidget = CreateWidget<UUserWidget>(GetWorld(), widget);
	if(spawnedWidget)
	{
		spawnedWidget->AddToViewport();
		spawnedWidget->SetVisibility(ESlateVisibility::Visible);
		return spawnedWidget;
	}
	

	return nullptr;
}

void ALiquidHUD::RemoveWidgetFromViewport(UUserWidget* widget)
{
	if(widget)
	{
		widget->RemoveFromParent();
	}
}
