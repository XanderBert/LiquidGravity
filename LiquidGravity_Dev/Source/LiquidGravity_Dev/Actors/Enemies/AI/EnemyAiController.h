#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyAiController.generated.h"


class ALiquidPlayer;
class UAISenseConfig_Sight;
class UAIPerceptionStimuliSourceComponent;
class UBehaviorTreeComponent;

UCLASS()
class LIQUIDGRAVITY_DEV_API AEnemyAiController : public AAIController
{
	GENERATED_BODY()
public:

	AEnemyAiController();
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;
	virtual void Tick(float DeltaSeconds) override;
	
	
	int32 PlayerKeyID{};
	int32 NextLocationID{};
	int32 SelfActorID{};

	//-------------------- AI Perception --------------------
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI Perception")
	UAIPerceptionComponent* AIPerceptionComponent;


	UFUNCTION()
	void OnPerceptionUpdated(const TArray<AActor*>& UpdatedActors);

	UPROPERTY(transient)
	UBlackboardComponent* BlackboardComp;

	UFUNCTION(BlueprintPure)
	bool GetPlayerDiscovered () const { return PlayerDiscovered; }
	UFUNCTION(BlueprintPure)
	bool GetPlayerDiscoveredAnimPlayed () const { return PlayerDiscoveredAnimPlayed; }

	UFUNCTION(BlueprintCallable)
	void SetPlayerDiscovered (bool value) { PlayerDiscovered = value; }
	UFUNCTION(BlueprintCallable)
	void SetPlayerDiscoveredAnimPlayed (bool value) { PlayerDiscoveredAnimPlayed = value; }
	
	// Cached BT component
	UPROPERTY(transient)
	UBehaviorTreeComponent* BehaviorComp;


private:
	UPROPERTY(EditAnywhere, Category = "Perception"	)
	float TouchDistance = 30.0f;

	UPROPERTY(EditAnywhere)
	bool PlayerDiscovered = false;

	UPROPERTY(EditAnywhere)
	bool PlayerDiscoveredAnimPlayed = false;
};
