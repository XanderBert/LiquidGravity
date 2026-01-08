#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GravityInterface.generated.h"

UINTERFACE(MinimalAPI, BlueprintType)
class UGravityInterface : public UInterface
{
	GENERATED_BODY()
};

class IGravityInterface
{
	GENERATED_BODY()
public:
	virtual FVector GetGravity(const FVector& position) = 0;
	virtual void SetGravity(const FVector& newGravity) = 0;
	
	bool IsComposite = false;
	IGravityInterface* CompositeGravityInterface;

	void Execute_SetGravityFieldActive(bool bCond, AActor* GravityFieldActor);
};
