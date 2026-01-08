#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/Pawn.h"
#include "LiquidPlayer.generated.h"

class UCameraHandler;
class ALiquidCamera;

UCLASS()
class LIQUIDGRAVITY_DEV_API ALiquidPlayer : public APawn
{
	GENERATED_BODY()

public:

	ALiquidPlayer();
	UFUNCTION()
	void OnPlayerDamaged(int Amount, AActor* DamagingActor, FVector DamageLocation);
	virtual void BeginPlay() override;
	

	
	FTimerHandle CheckHighItensityTimer;
	UFUNCTION()
	void SetHighIntensityMusicWhenNeeded();

	
	//-------------------- Components --------------------
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	class UCapsuleComponent* CapsuleComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USkeletalMeshComponent* SkeletalMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USphereComponent* DefaultOverlappingSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USphereComponent* FootOverlappingSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class ULiquidMovementComponent* LiquidMovementComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UStateTreeComponent* StateTreeComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UChangeGravityComponent* ChangeGravityComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UHealthComponent* HealthComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UEnergyComponent* EnergyComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UCombatComponent* CombatComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UPlayerHookingComponent* HookingComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UInteractComponent* InteractComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UPickupComponent* PickupComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UPlayerStuntComponent* StuntComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UPlayerFeedbackComponent* FeedbackComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly , Category = "Components")
	UAudioComponent* SkatingSound;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly , Category = "Components")
	UAudioComponent* GrindingSound;

	//------------ Music ------------
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly , Category = "Components")
	UAudioComponent* MusicComponent;

	UPROPERTY(EditAnywhere, Category = "Music")
	USoundBase* MenuMusic;

	UPROPERTY(EditAnywhere, Category = "Music")
	USoundBase* GameMusic;

	//-------------------- Camera --------------------

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly , Category = "Components")
	UCameraHandler* CameraHandler;
	

	//-------------------- Gameplay Tags --------------------
	UPROPERTY(EditAnywhere, Category = "Gameplay Tags")
	FGameplayTagContainer GameplayTags;

private:
	
	

	//-------------------- Music --------------------
	UFUNCTION()
	void SetupMusicAndPlay();
	
	UFUNCTION()
	void SetMusicToMenu();
	
	UFUNCTION()
	void SetMusicToGame();

	UFUNCTION()
	void SetHighIntensityMusic();

	UFUNCTION()
	void SetLowIntensityMusic();

	UFUNCTION()
	void SetToMediumHighForPeriod(int Amount, AActor* DamagingActor, FVector DamageLocation);

};
