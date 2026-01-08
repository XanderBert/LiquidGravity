#include "HoldToSkipWidget.h"

#include "LiquidGravity_Dev/Core/LiquidHUD.h"

void UHoldToSkipWidget::StartHoldToSkip()
{
	bIsHoldingToSkip = true;
}

void UHoldToSkipWidget::StopHoldToSkip()
{
	bIsHoldingToSkip = false;
	TimeHeldDown = 0;
}

void UHoldToSkipWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	if(bIsHoldingToSkip)
	{
		TimeHeldDown += InDeltaTime;
		if(TimeHeldDown >= HoldTime)
		{
			//GetHUD
			ALiquidHUD* LiquidHUD = Cast<ALiquidHUD>(GetOwningPlayer()->GetHUD());
			LiquidHUD->OnSkipped.Execute();
			StopHoldToSkip();
		}
	}
}
