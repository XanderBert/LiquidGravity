#include "GravityInterface.h"

#include "LiquidGravity_Dev/Core/LiquidGameInstance.h"


void IGravityInterface::Execute_SetGravityFieldActive(bool bCond, AActor* GravityFieldActor)
{
	ULiquidGameInstance* LiquidGameInstance = Cast<ULiquidGameInstance>(GWorld->GetGameInstance());
	
	if (bCond)
	{
		LiquidGameInstance->Gravity.SetGravitySource(this, GravityFieldActor);
		LiquidGameInstance->Gravity.SetIsForcedLockedGravity(false, FVector::Zero());
	}
}