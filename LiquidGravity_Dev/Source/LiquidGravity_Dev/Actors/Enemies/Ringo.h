#pragma once

#include "CoreMinimal.h"
#include "BaseEnemy.h"
#include "Ringo.generated.h"

UCLASS()
class LIQUIDGRAVITY_DEV_API ARingo : public ABaseEnemy
{
	GENERATED_BODY()

	
public:

	ARingo();
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
};
