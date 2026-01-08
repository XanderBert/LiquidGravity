#pragma once

#include "CoreMinimal.h"
#include "EnergyAdditionDataTable.generated.h"


USTRUCT()
struct FEnergyAdditionDataTable : public FTableRowBase
{
	GENERATED_BODY()
public:
	
	UPROPERTY(EditAnywhere)
	float EnergyAddition{5.0f};

	UPROPERTY(EditAnywhere)
	float TimeInterval{1.0f};
};