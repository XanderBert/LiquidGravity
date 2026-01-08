#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "LiquidHUD.generated.h"

class UHoldToSkipWidget;
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDisableMenuSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPauseSignature);

DECLARE_DYNAMIC_DELEGATE(FOnHoldToSkipSignature);

UCLASS()
class LIQUIDGRAVITY_DEV_API ALiquidHUD : public AHUD
{
	GENERATED_BODY()
public:
	virtual void BeginPlay() override;

	void SetupTrailerHUD();

	UFUNCTION()
	void SetupMainMenu();

	UFUNCTION(BlueprintCallable)
	void OnStartMenuInputReceived();

	UFUNCTION(BlueprintCallable)
	void PauseMenu();

	UFUNCTION(BlueprintCallable)
	void UnpauseMenu();

	UFUNCTION(BlueprintCallable)
	void RemovePlayerHUD();

	UFUNCTION(BlueprintCallable)
	void AddPlayerHUD();


	UFUNCTION(BlueprintCallable)
	UUserWidget* ShowEndScreen();
	
	UFUNCTION(BlueprintPure)
	UUserWidget* GetPlayerHUD() const { return PlayerHUD; }

	FOnDisableMenuSignature OnDisableMenu;
	
	FOnHoldToSkipSignature OnSkipped;
	FOnPauseSignature OnPause;
	FOnPauseSignature OnUnpause;

	//-------------------------TRAILER HUD-------------------------
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite, Category = "Widgets")
	UHoldToSkipWidget* TrailerHUD{};

	//-------------------------MAIN HUD-------------------------
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Widgets")
	UUserWidget* MainMenuHUD{};

	//-------------------------PLAYER HUD-------------------------
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite, Category = "Widgets")
	UUserWidget* PlayerHUD{};


	//-------------------------PLAYER HUD-------------------------
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite, Category = "Widgets")
	UUserWidget* EndingScreen{};
private:

	//-------------------------TRAILER HUD-------------------------
	UPROPERTY(EditAnywhere, Category = "Widgets")
	TSubclassOf<class UUserWidget> TrailerHUDClass;
	
	//-------------------------Ending Screen -------------------------
	UPROPERTY(EditAnywhere, Category = "Widgets")
	TSubclassOf<class UUserWidget> PlayerHUDClass;

	//-------------------------Ending Screen -------------------------
	UPROPERTY(EditAnywhere, Category = "Widgets")
	TSubclassOf<class UUserWidget> EndingScreenClass{};

	//-------------------------MAIN HUD-------------------------
	UPROPERTY(EditAnywhere, Category = "Widgets")
	TSubclassOf<class UUserWidget> MainMenuHUDClass;
	


	//-------------------------PAUSE HUD-------------------------
	UPROPERTY(EditAnywhere, Category = "Widgets")
	TSubclassOf<class UUserWidget> PauseMenuClass;
	UPROPERTY(VisibleAnywhere, Category = "Widgets")
	UUserWidget* PauseMenuWidget{};





	
	UUserWidget* AddWidgetToViewport(TSubclassOf<class UUserWidget> widget) const;
	static void RemoveWidgetFromViewport(UUserWidget* widget);


	bool bIsMainMenuActive = false;
};
