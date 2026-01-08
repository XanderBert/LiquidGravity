#pragma once

#include "CoreMinimal.h"
#include "UStuntEnum.h"
#include "FlickInputSettings.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFlickPerformedSignature, UStuntEnum, StuntEnum);

USTRUCT(BlueprintType)
struct LIQUIDGRAVITY_DEV_API FFlickInputSettings 
{
	GENERATED_BODY()

	FFlickInputSettings();
	
	//The forwarded value is supposed to be between -1 and 1 range
	void ForwardInput(float value);
	void Reset();

	void Tick(float DeltaTime);
	
	UPROPERTY(EditAnywhere, Category = "Flick")
	float TimeToPerformFlick{0.3f};

	UPROPERTY(EditAnywhere, Category = "Flick")
	bool bFirstUpThenDown{true};

	UPROPERTY(EditAnywhere, Category = "Flick")
	UStuntEnum StuntEnum = UStuntEnum::GagnamStyle;

	FOnFlickPerformedSignature OnFlickPerformed;

private:
	static bool IsValueUp(float value);
	static bool IsValueDown(float value);

	bool bFlickPossible = true;
	
	bool bReachedUp{};
	bool bReachedDown{};
	
	//bool bIsInputActive{};
	// FTimerHandle TimerHandle{};
	// FTimerDelegate TimerDelegate{};
	//float elapsedTime{};
	float GlobalTimeWhenInputStarted{};

	float axisValue{};

};
