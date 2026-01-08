// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "Components/WidgetComponent.h"
#include "ChangeGravityComponent.generated.h"


class UNiagaraSystem;
class UNiagaraComponent;
class IGravityInterface;
class ULiquidGameInstance;
class ALiquidPlayer;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LIQUIDGRAVITY_DEV_API UChangeGravityComponent : public UWidgetComponent
{
	GENERATED_BODY()

public:

	UChangeGravityComponent();
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	void HandleGravityFX(float DeltaTime);
	bool IsValidSurface(IGravityInterface* currentHit, const FHitResult* hit) const;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void SetLookedAtGravityFieldActive();

	void AddChangeGravityTag() const;
	void RemoveChangeGravityTag() const;

	FVector GetTargetPosition() const
	{ return CachedTargetPosition; }

private:
	UPROPERTY()
	ALiquidPlayer* LiquidPlayer{};

	UPROPERTY()
	ULiquidGameInstance* LiquidGameInstance{};

	UPROPERTY()
	AActor* currentGravFieldActor{};

	UPROPERTY(EditAnywhere)
	FGameplayTag ChangeGravityTag;

	UPROPERTY(EditAnywhere)
	UNiagaraSystem* GravityDirectionFX{};

	UPROPERTY()
	UNiagaraComponent* GravityDirectionNiagaraComponent{};

	UPROPERTY(EditAnywhere)
    float MinimumDistance = 500.f;

	UPROPERTY(EditAnywhere)
	float MaximumDistance = 2000.f;
	
	FVector TargetPosition{};

	FVector CachedTargetPosition{};

	FVector GravityDirectionEndPosition{};
	
	IGravityInterface* CurrentGravityField{};

	FVector CurrentHitLocation{};
};