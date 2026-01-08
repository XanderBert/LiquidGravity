// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "LiquidGravity_Dev/Stunts/UStuntEnum.h"
#include "PlayerStuntComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnScoreAddedSignature);

struct FStuntStruct
{
	FString StuntName{};
	int ScoreAddition = 500;
	float energyAddition = 20.f;
	AActor* contextActor = nullptr;
};

class ALiquidPlayer;
enum class UStuntEnum : uint8;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LIQUIDGRAVITY_DEV_API UPlayerStuntComponent : public UActorComponent
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
	
public:
	UPROPERTY(BlueprintAssignable)
	FOnScoreAddedSignature OnScoreAdded;
	UPROPERTY(BlueprintAssignable)
	FOnScoreAddedSignature OnStuntTextAdded;
	
	UPlayerStuntComponent();

	void SetStuntString();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	void StartStunt(UStuntEnum stuntEnum);

	UFUNCTION(BlueprintCallable)
	void CalculateFinalScore();

	UStuntEnum GetCurrentStunt() const { return CurrentStunt; }

	float GetStuntDuration(UStuntEnum stuntEnum);

	UFUNCTION(BlueprintPure)
	float GetTimePerStunt() const
	{
		return TimePerStunt;
	}
	UFUNCTION(BlueprintPure)
	float GetStuntTimer() const
	{
		return CurrentTimer;
	}
	
	UFUNCTION(BlueprintPure)
	float GetCurrentMultiplier() const
	{
		if (CurrentStunts.Num() > 0)
			return CurrentStunts.Num() * 0.25 + 0.75f;

		return 0;
	}

	void AddStuntTag() const;
	void RemoveStuntTag();
	bool IsStuntSafe() const;
	FGameplayTag& GetStuntTag();

	UFUNCTION(BlueprintPure)
	FString GetStuntString()	
	{
		return StuntString;
	}

	UFUNCTION(BlueprintPure)
	int GetScore() const
	{
		return StuntScore;
	}

	void AddStuntStruct(FStuntStruct& stuntStruct);
	void FailStunt();
	void ResetStuntString();

private:
	UStuntEnum CurrentStunt = UStuntEnum::GagnamStyle;
	
	UPROPERTY(EditAnywhere)
	TMap<UStuntEnum, float> StuntDuration;

	UPROPERTY(EditAnywhere)
	FGameplayTag StuntTag;

	UPROPERTY()
	ALiquidPlayer* LiquidPlayer = nullptr;

	bool IsStunting = false;
	
	bool CanStunt () const;

	float ComboMultiplier = 1.f;

	FString StuntString = "";

	int StuntScore{};
	TArray<FStuntStruct> CurrentStunts{};
	
	void AddScore(int score);

	float CurrentTimer{};
	UPROPERTY(EditAnywhere)
	float TimePerStunt = 7.f;
};
