#pragma once

#include "CoreMinimal.h"
#include "CustomGravity.h"
#include "GameStateEnum.h"
#include "LiquidController.h"
#include "LiquidPlayer.h"
#include "Components/AudioComponent.h"
#include "Engine/GameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "LiquidGameInstance.generated.h"

DECLARE_DYNAMIC_DELEGATE(FOnLowSettingsSignature);
DECLARE_DYNAMIC_DELEGATE(FOnHighSettingsSignature);


enum class GameStateEnum : uint8;


UCLASS(BlueprintType)
class UValueGraphicsSetting : public UObject
{
	GENERATED_BODY()
	
public:	
	int Value = 0;
	FString Command{};

	UFUNCTION(BlueprintPure)
	bool IsEnabled() const { return Value > 0; }

	UFUNCTION(BlueprintPure)
	int GetValue() const { return Value; }

	
	UFUNCTION(BlueprintCallable)
	void SetValue(int value)
	{
		APlayerController* controller = UGameplayStatics::GetPlayerController(GetWorld(),0);
		
		Value = value;

		if(controller)
		{
			controller->ConsoleCommand(Command + " " + FString::FromInt(value));
		}
		
	}
};


UCLASS(BlueprintType)
class UGraphicsSettings : public UObject
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	void ApplyLowSettings(){ OnLowSettings.Execute(); }

	UFUNCTION(BlueprintCallable)
	void ApplyHighSettings(){ OnHighSettings.Execute(); }
	
	FOnLowSettingsSignature OnLowSettings;
	FOnHighSettingsSignature OnHighSettings;
	
	UValueGraphicsSetting* Create(const FName& name, int32 InitialValue, const FString& ConsoleCommand)
	{
		UValueGraphicsSetting* newSetting = NewObject<UValueGraphicsSetting>(this, name);//CreateDefaultSubobject<UValueGraphicsSetting>(name);
		newSetting->Value = InitialValue;
		newSetting->Command = ConsoleCommand;
		return newSetting;
	}

	UFUNCTION(BlueprintCallable)
	void AddSetting(const FName& name, int initialValue, const FString& consoleCommand)
	{
		auto newSetting = Settings.Add(name, Create(name, initialValue, consoleCommand));
		newSetting->SetValue(initialValue);
	}

	UFUNCTION(BlueprintPure)
	UValueGraphicsSetting* GetSetting(FName name) const { return Settings.FindRef(name); }

	UFUNCTION(BlueprintPure)
	TMap<FName, UValueGraphicsSetting*> GetSettings() const { return Settings; }
	
	UPROPERTY(EditAnywhere, Category = "Graphics Settings")
	TMap<FName, UValueGraphicsSetting*> Settings;
	
};


UCLASS()
class LIQUIDGRAVITY_DEV_API ULiquidGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:
	ULiquidGameInstance();

	virtual void Init() override;


	UFUNCTION(BlueprintCallable)
	TArray<AActor*> GetRingoActors() const;
	
	UFUNCTION(BlueprintPure)
	int GetAliveRingoCount() const;
	
	UFUNCTION(BlueprintPure)
	UGraphicsSettings* GetGraphicsSettings() const { return GraphicsSettings; }
	
	UPROPERTY(EditAnywhere, Category = "Graphics Settings")
	UGraphicsSettings* GraphicsSettings{};

	UFUNCTION()
	void SetLowSettings();

	UFUNCTION()
	void SetHighSettings();
	
	UFUNCTION(BlueprintCallable)
	UUserWidget* RemovePlayerHUDAndShowEndScreen();

	UFUNCTION(BlueprintPure)
	GameStateEnum GetCurrentGameState() const { return CurrentGameState; }
	UFUNCTION(BlueprintCallable)
	void SetCurrentGameState(GameStateEnum state) { CurrentGameState = state; }

	UFUNCTION(BlueprintCallable)
	void ReloadCurrentLevel();
	
	UFUNCTION(BlueprintCallable)
	void SetScreenShakeIntensity(float intensity) { ScreenShakeIntensity = intensity; }

	UFUNCTION(BlueprintPure)
	float GetScreenShakeIntensity() const { return ScreenShakeIntensity; }
	
	UFUNCTION(BlueprintCallable)
	void SetScreenShakeEnabled(bool enabled) { ScreenShakeEnabled = enabled; }

	UFUNCTION(BlueprintPure)
	bool GetScreenShakeEnabled() const { return ScreenShakeEnabled; }

	
	UFUNCTION(BlueprintCallable)
	void SetVibrationEnabled(bool enabled);

	UFUNCTION(BlueprintPure)
	bool GetVibrationEnabled() const { return VibrationEnabled; }

	
	UFUNCTION(BlueprintCallable)
	void SetMusicVolume(float volume);

	UFUNCTION(BlueprintCallable)
	void SetCameraSenstivity(float senstivity);

	UFUNCTION(BlueprintPure)
	float GetMusicVolume() const { return MusicVolume; }

	UFUNCTION(BlueprintPure)
	float GetCameraSenstivity() const { return CameraSensitivity; }
	
	UFUNCTION(BlueprintCallable)
	void SetSoundVolume(float volume) { SoundVolume = volume; }

	UFUNCTION(BlueprintPure)
	float GetSoundVolume() const { return SoundVolume; }

	//Scoring/Grading Shit
	float HighestCombo = 0.f;
	int Score = 0;
	int EnemiesKilled = 0;
	float DamageTaken = 0;

	UFUNCTION(BlueprintPure)
	int GetHighestComboGrade() const;

	UFUNCTION(BlueprintPure)
	FString GetHighestCombo() const
	{
		return FString::SanitizeFloat(HighestCombo) + "x";
	}

	UFUNCTION(BlueprintPure)
	int GetScoreGrade() const;

	UFUNCTION(BlueprintPure)
	FString GetScore() const
	{
		return FString::FromInt(Score) + "PTs";
	}

	UFUNCTION(BlueprintPure)
	int GetEnemiesKilledGrade() const;

	UFUNCTION(BlueprintPure)
	FString GetEnemiesKilled() const
	{
		return FString::FromInt(EnemiesKilled);
	}
	
	UFUNCTION(BlueprintPure)
	int GetDamageTakenGrade() const;

	UFUNCTION(BlueprintPure)
	FString GetDamageTaken() const
	{
		return FString::FromInt(DamageTaken);
	}

	UFUNCTION(BlueprintPure)
	int GetOverallGrade() const;

	UFUNCTION(BlueprintPure)
	static FString GetGradeString(int grade);

	UFUNCTION(BlueprintPure)
	FString GetEndGameString();
	
	UPROPERTY(EditAnywhere, Category = "Gravity")
	FCustomGravity Gravity{};

	UPROPERTY(EditAnywhere, Category = "Feedback")
	float ScreenShakeIntensity = 1.0f;

	UPROPERTY(EditAnywhere, Category = "Feedback")
	bool ScreenShakeEnabled = true;

	UPROPERTY(EditAnywhere, Category = "Feedback")
	bool VibrationEnabled = true;

	UPROPERTY(EditAnywhere, Category = "Feedback")
	float MusicVolume = 0.5;

	UPROPERTY(EditAnywhere, Category = "Feedback")
	float SoundVolume = 1.f;

	UPROPERTY(EditAnywhere, Category = "Feedback")
	float CameraSensitivity = 1.f;
	
	UPROPERTY(EditAnywhere)
	USoundAttenuation* SoundAttenuation{};

	UPROPERTY(EditAnywhere, Category = "Music")
	class USoundBase* Music;

	GameStateEnum CurrentGameState = GameStateEnum::InGame;
};
