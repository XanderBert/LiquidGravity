#include "LiquidGameInstance.h"

#include "LiquidHUD.h"
#include "LiquidGravity_Dev/Actors/Enemies/BaseEnemy.h"
#include "LiquidGravity_Dev/Actors/Enemies/EnemyBullet.h"
#include "LiquidGravity_Dev/Actors/Enemies/Ringo.h"

ULiquidGameInstance::ULiquidGameInstance()
{
	GraphicsSettings = CreateDefaultSubobject<UGraphicsSettings>("Graphics Settings");
}

void ULiquidGameInstance::Init()
{
	Super::Init();
	
	GraphicsSettings->AddSetting("VSync",0, "r.VSync");
	GraphicsSettings->AddSetting("Bloom", 10, "r.BloomQuality");
	GraphicsSettings->AddSetting("LensFlare", 1, "r.DefaultFeature.LensFlare");

	GraphicsSettings->AddSetting("TemporalAA", 8, "r.TemporalAASamples");
	GraphicsSettings->AddSetting("LumenProbeResolution", 16, "r.Lumen.ScreenProbeGather.RadianceCache.ProbeResolution");
	GraphicsSettings->AddSetting("LumenDownsampleFactor", 32, "r.Lumen.ScreenProbeGather.DownsampleFactor");
	GraphicsSettings->AddSetting("ShadowQuality", 2, "sg.ShadowQuality");
	GraphicsSettings->AddSetting("EffectQuality", 3, "sg.EffectsQuality");
	GraphicsSettings->AddSetting("DetailMode", 2, "r.DetailMode");
	GraphicsSettings->AddSetting("TextureQuality", 3, "sg.TextureQuality");
	GraphicsSettings->AddSetting("PostProcessQuality", 3, "sg.PostProcessQuality");
	GraphicsSettings->AddSetting("FoliageQuality", 3, "FoliageQuality");
	
	GraphicsSettings->AddSetting("AAQuality", 5, "r.PostProcessAAQuality");
	GraphicsSettings->AddSetting("ResolutionScale", 100, "r.ScreenPercentage");


	
	GraphicsSettings->AddSetting("Reflections", 0, "r.lumen.reflections.Allow");

	GraphicsSettings->OnLowSettings.BindDynamic(this, &ULiquidGameInstance::SetLowSettings);
	GraphicsSettings->OnHighSettings.BindDynamic(this, &ULiquidGameInstance::SetHighSettings);
}

TArray<AActor*> ULiquidGameInstance::GetRingoActors() const
{
	//Get all ARingos
	TArray<AActor*> ringos;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ARingo::StaticClass(), ringos);

	return ringos;
}

int ULiquidGameInstance::GetAliveRingoCount() const
{
	auto ringos = GetRingoActors();

	//Check if they are still valid -> Marked for deletion
	for(auto& ringo : ringos)
	{
		if(!IsValid(ringo))
		{
			ringos.Remove(ringo);
		}
	}

	
	return ringos.Num();
}

void ULiquidGameInstance::SetLowSettings()
{
	GraphicsSettings->GetSetting("Bloom")->SetValue(0);
	GraphicsSettings->GetSetting("LensFlare")->SetValue(0);
	GraphicsSettings->GetSetting("TemporalAA")->SetValue(4);
	GraphicsSettings->GetSetting("LumenProbeResolution")->SetValue(8);
	GraphicsSettings->GetSetting("LumenDownsampleFactor")->SetValue(64);
	GraphicsSettings->GetSetting("ShadowQuality")->SetValue(1);
	GraphicsSettings->GetSetting("EffectQuality")->SetValue(0);
	GraphicsSettings->GetSetting("DetailMode")->SetValue(0);
	GraphicsSettings->GetSetting("TextureQuality")->SetValue(0);
	GraphicsSettings->GetSetting("PostProcessQuality")->SetValue(0);
	GraphicsSettings->GetSetting("FoliageQuality")->SetValue(0);
	GraphicsSettings->GetSetting("AAQuality")->SetValue(1);
	GraphicsSettings->GetSetting("ResolutionScale")->SetValue(75);
}

void ULiquidGameInstance::SetHighSettings()
{
	GraphicsSettings->GetSetting("Bloom")->SetValue(10);
	GraphicsSettings->GetSetting("LensFlare")->SetValue(1);
	GraphicsSettings->GetSetting("TemporalAA")->SetValue(8);
	GraphicsSettings->GetSetting("LumenProbeResolution")->SetValue(16);
	GraphicsSettings->GetSetting("LumenDownsampleFactor")->SetValue(32);
	GraphicsSettings->GetSetting("ShadowQuality")->SetValue(2);
	GraphicsSettings->GetSetting("EffectQuality")->SetValue(3);
	GraphicsSettings->GetSetting("DetailMode")->SetValue(2);
	GraphicsSettings->GetSetting("TextureQuality")->SetValue(3);
	GraphicsSettings->GetSetting("PostProcessQuality")->SetValue(3);
	GraphicsSettings->GetSetting("FoliageQuality")->SetValue(3);
	GraphicsSettings->GetSetting("AAQuality")->SetValue(5);
	GraphicsSettings->GetSetting("ResolutionScale")->SetValue(100);
}

UUserWidget* ULiquidGameInstance::RemovePlayerHUDAndShowEndScreen()
{
	//Get LiquidHUD
	ALiquidHUD* hud = Cast<ALiquidHUD>(UGameplayStatics::GetPlayerController(GetWorld(),0)->GetHUD());

	hud->RemovePlayerHUD();
	return hud->ShowEndScreen();
}

void ULiquidGameInstance::ReloadCurrentLevel()
{
	//Get Current level name
	HighestCombo = 0;
	Score = 0;
	EnemiesKilled = 0;
	DamageTaken = 0;

	CurrentGameState = GameStateEnum::InGame;
	FString levelName = UGameplayStatics::GetCurrentLevelName(GetWorld(),true);
	FName levelNameFname = FName(*levelName);
	UGameplayStatics::OpenLevel(GetWorld(),levelNameFname);
}

void ULiquidGameInstance::SetVibrationEnabled(bool enabled)
{
	VibrationEnabled = enabled;
	if (enabled == false)
	{
		UGameplayStatics::GetPlayerController(GetWorld(),0)->ClientStopForceFeedback(nullptr,"");
	}
}

void ULiquidGameInstance::SetMusicVolume(float volume)
{
	MusicVolume = volume;
		
	Cast<ALiquidPlayer>(UGameplayStatics::GetPlayerController(GetWorld(),0)->GetPawn())->MusicComponent->SetVolumeMultiplier(MusicVolume);
}

void ULiquidGameInstance::SetCameraSenstivity(float senstivity)
{
	CameraSensitivity = senstivity;
}

int ULiquidGameInstance::GetHighestComboGrade() const
{
	if (HighestCombo > 7.5f)
	{
		return 0;
	}
	else if (HighestCombo > 5.f)
	{
		return 1;
	}
	else if (HighestCombo > 3.f)
	{
		return 2;
	}
	else if (HighestCombo > 2.f)
	{
		return 3;
	}
	else
	{
		return 4;
	}
}

int ULiquidGameInstance::GetScoreGrade() const
{
	if (Score > 200000)
	{
		return 0;
	}
	else if (Score > 125000)
	{
		return 1;
	}
	else if (Score > 75000)
	{
		return 2;
	}
	else if (Score > 25000)
	{
		return 3;
	}
	else
	{
		return 4;
	}
}

int ULiquidGameInstance::GetEnemiesKilledGrade() const
{
	if (EnemiesKilled > 40)
	{
		return 0;
	}
	else if (EnemiesKilled > 25)
	{
		return 1;
	}
	else if (EnemiesKilled > 15)
	{
		return 2;
	}
	else if (EnemiesKilled > 10)
	{
		return 3;
	}
	else
	{
		return 4;
	}
}

int ULiquidGameInstance::GetDamageTakenGrade() const
{
	if (DamageTaken <= 100.f)
	{
		return 0;
	}
	else if (DamageTaken < 300.f)
	{
		return 1;
	}
	else if (DamageTaken < 500.f)
	{
		return 2;
	}
	else if (DamageTaken < 700.f)
	{
		return 3;
	}
	else
	{
		return 4;
	}
}

int ULiquidGameInstance::GetOverallGrade() const
{
	return (GetHighestComboGrade() + GetScoreGrade() + GetEnemiesKilledGrade() + GetDamageTakenGrade())/4;
}

FString ULiquidGameInstance::GetGradeString(int grade)
{
	switch (grade)
	{
	case 0:
		return FString("S");
	case 1:
		return FString("A");
	case 2:
		return FString("B");
	case 3:
		return FString("C");
	default:
		return FString("D");
	}
}

FString ULiquidGameInstance::GetEndGameString()
{
	FString endGameString;
	
	endGameString += "Overall Grade: " + GetGradeString(GetOverallGrade()) + "\n\n";

	endGameString += "Highest Combo: " + FString::SanitizeFloat(HighestCombo) + "x Grade: " + GetGradeString(GetHighestComboGrade()) + "\n";
	endGameString += "Score: " + FString::FromInt(Score) + "PTs Grade: " + GetGradeString(GetScoreGrade()) + "\n";
	endGameString += "Enemies Killed: " + FString::FromInt(EnemiesKilled) + " Grade: " + GetGradeString(GetEnemiesKilledGrade()) + "\n";
	endGameString += "Damage Taken: " + FString::FromInt(static_cast<int>(DamageTaken)) + " Grade: " + GetGradeString(GetDamageTakenGrade()) + "\n";

	return endGameString;
}
