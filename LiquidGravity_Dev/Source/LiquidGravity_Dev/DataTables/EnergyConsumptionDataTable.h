#pragma once

#include "CoreMinimal.h"
#include "EnergyConsumptionDataTable.generated.h"


USTRUCT()
struct FEnergyConsumptionRow : public FTableRowBase
{
	GENERATED_BODY()
public:
	
	UPROPERTY(EditAnywhere)
	float EnergyConsumption{5.0f};
};