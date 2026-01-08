#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HoldToSkipWidget.generated.h"



UCLASS()
class LIQUIDGRAVITY_DEV_API UHoldToSkipWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	void StartHoldToSkip();
	void StopHoldToSkip();
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HoldToSkip")
	float HoldTime = 2.5f;

	UFUNCTION(BlueprintPure, Category = "HoldToSkip")
	bool IsHoldingToSkip() const { return bIsHoldingToSkip; }

	UFUNCTION(BlueprintPure, Category = "HoldToSkip")
	float GetHeldDownTime() const { return TimeHeldDown; }

	UFUNCTION(BlueprintPure, Category = "HoldToSkip")
	float GetHeldDownPercentage() const { return TimeHeldDown / HoldTime; }

	
	
private:
	float TimeHeldDown{};
	bool bIsHoldingToSkip{};
};
