#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "BaseEnemy.generated.h"

class AEnemySpawner;
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerSpottedSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerLostSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeadOnThrowSignature);

class UNiagaraSystem;
class UAIPerceptionStimuliSourceComponent;

UENUM(BlueprintType)
enum class UAnimationState : uint8
{
	Idle UMETA(DisplayName = "Idle"),
	Moving UMETA(DisplayName = "Moving"),
	Attacking UMETA(DisplayName = "Attacking"),
	PlayerDiscovered UMETA(DisplayName = "PlayerDiscovered"),
	HeavyAttack UMETA(DisplayName = "HeavyAttack"),
	Count UMETA(Hidden),
  };

UCLASS()
class LIQUIDGRAVITY_DEV_API ABaseEnemy : public APawn
{
	GENERATED_BODY()

public:
	ABaseEnemy();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	//-------------------- Delegates --------------------
	UPROPERTY(BlueprintAssignable)
	FOnPlayerSpottedSignature OnPlayerSpotted;

	UPROPERTY(BlueprintAssignable)
	FOnPlayerLostSignature OnPlayerLost;

	UPROPERTY(BlueprintAssignable)
	FOnDeadOnThrowSignature OnDeadOnThrow;
	
	//-------------------- Components --------------------
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USkeletalMeshComponent* SkeletalMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UCapsuleComponent* CapsuleComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UAIPerceptionStimuliSourceComponent* AIPerceptionStimuliSourceComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	class UEnemyShootingComponent* EnemyShootingComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	class UHealthComponent* HealthComponent;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	class UWaypointUsageComponent* WaypointUsageComponent;

	//-------------------- Members --------------------
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
	class UBehaviorTree* BehaviorTree;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
	UAnimationState CurrentAnimationState = UAnimationState::Moving;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DeadBody")
	TSubclassOf<AActor> DeadBody;

	UPROPERTY(EditAnywhere, Category = "DeadBody")
	UNiagaraSystem* DeathFX;

	UPROPERTY(EditAnywhere, Category = "DeadBody")
	bool bSpawnDeadBody = true;

	UPROPERTY(EditAnywhere)
	FString EnemyName = "Enemy";
	UPROPERTY(EditAnywhere)
	float ScoreOnAddition = 500.f;
	
	//-------------------- Functions --------------------
	UFUNCTION(BlueprintPure)
	bool GetTookDamage () const { return TookDamage; }
	
	UFUNCTION(BlueprintCallable)
	void SetTookDamage (bool value) { TookDamage = value; }

	UFUNCTION(BlueprintCallable)
	void EnableKick(const FVector& impulse);

	UFUNCTION(BlueprintCallable)
	void DisableKick();
	
	UFUNCTION(BlueprintCallable)
	bool GetIsKick() const;

	float RandomAnimationTimer = 0.f;

	UFUNCTION(BlueprintCallable)
	void NotifyEnemiesInRange();	


	void PlaySound(USoundBase* Sound, float Volume = 1.f, float minPitch = 0.8f, float maxPitch = 1.2f) const;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UAudioComponent* EnemySound;

	UPROPERTY(EditAnywhere)
	TArray<USoundBase*> HitSounds;

	UPROPERTY(EditAnywhere)
	TArray<USoundBase*> DeathSounds;

	UPROPERTY(EditAnywhere)
	TArray<USoundBase*> IdleSounds;

	UFUNCTION()
	float GetKickMultiplier();

	UPROPERTY()
	AEnemySpawner* SpawnedBy;


	UFUNCTION(BlueprintPure)
	bool SpotsPlayer() const
	{
		return bSpotsPlayer;
	}

	UFUNCTION()
	void SetSpotsPlayer()
	{
		bSpotsPlayer = true;
	}

	UFUNCTION()
	void SetLostPlayer()
	{
		bSpotsPlayer = false;
	}

	
private:

	bool bSpotsPlayer;

	UFUNCTION()
	void NotifyAISystem();


	UFUNCTION()
	void NotifyAISystemOnShoot(int amount, AActor* damagingActor, FVector damageLocation);
	
	UFUNCTION()
	void DestroyEnemy();

	bool TookDamage = false;

	bool IsBeingKick = false;

	UPROPERTY(EditAnywhere, Category = "Kicking")
	float KickForceMultiplier = 1.0f;

	UPROPERTY(EditAnywhere, Category = "AI")
	float NotifyOtherEnemiesRange = 2500.f;
};
