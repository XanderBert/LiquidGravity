// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerStuntComponent.h"

#include "LiquidMovementComponent.h"
#include "PlayerFeedbackComponent.h"
#include "LiquidGravity_Dev/Components/EnergyComponent.h"
#include "LiquidGravity_Dev/Components/HealthComponent.h"
#include "LiquidGravity_Dev/Core/LiquidGameInstance.h"
#include "LiquidGravity_Dev/Core/LiquidPlayer.h"


// Sets default values for this component's properties
UPlayerStuntComponent::UPlayerStuntComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UPlayerStuntComponent::SetStuntString()
{
	StuntString = "";
	if (CurrentStunts.Num() > 0)
	{
		StuntString.Append(CurrentStunts[CurrentStunts.Num()-1].StuntName);
		StuntString.Append("\n");
		for(int i = 0; i < CurrentStunts.Num(); i++)
		{
			StuntString.AppendInt(CurrentStunts[i].ScoreAddition);
			if (i < CurrentStunts.Num()-1)
			{
				StuntString.Append(" + ");
			}
		}
	}
	OnStuntTextAdded.Broadcast();
}


// Called when the game starts
void UPlayerStuntComponent::BeginPlay()
{
	Super::BeginPlay();

	LiquidPlayer = Cast<ALiquidPlayer>(GetOwner());
	CurrentTimer = GetTimePerStunt();
}


// Called every frame
void UPlayerStuntComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                          FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!LiquidPlayer->LiquidMovementComponent->IsGrinding())
		CurrentTimer += DeltaTime;

	if (CurrentStunts.Num() > 0 && !LiquidPlayer->LiquidMovementComponent->IsGrinding())
	{
		if (CurrentTimer > TimePerStunt)
		{
			ULiquidGameInstance* gameInstance = Cast<ULiquidGameInstance>(GetWorld()->GetGameInstance());

			if(IsValid(gameInstance))
			{
				if(GetCurrentMultiplier() > gameInstance->HighestCombo)
				{
					gameInstance->HighestCombo = GetCurrentMultiplier();
				}
			}
			
			for (auto stuntStruct : CurrentStunts)
			{
				AddScore(stuntStruct.ScoreAddition * GetCurrentMultiplier());
			}
		
			CurrentStunts.Empty();
		}
	}
}

void UPlayerStuntComponent::StartStunt(UStuntEnum stuntEnum)
{
	if (!CanStunt())
		return;
	
	CurrentStunt = stuntEnum;
	IsStunting = true;

	AddStuntTag();
}

void UPlayerStuntComponent::CalculateFinalScore()
{
	CurrentTimer = GetTimePerStunt();
}

float UPlayerStuntComponent::GetStuntDuration(UStuntEnum stuntEnum)
{
	if (StuntDuration.Contains(stuntEnum))
	{
		return StuntDuration[stuntEnum];
	}
	return 0;
}

void UPlayerStuntComponent::AddStuntTag() const
{
	LiquidPlayer->GameplayTags.AddTag(StuntTag);
}

void UPlayerStuntComponent::RemoveStuntTag()
{
	LiquidPlayer->GameplayTags.RemoveTag(StuntTag);
	IsStunting = false;
}

bool UPlayerStuntComponent::IsStuntSafe() const
{
	if (!IsValid(LiquidPlayer))
		return false;

	ULiquidMovementComponent* MovementComponent = LiquidPlayer->LiquidMovementComponent;

	//TODO: Stop Stunting if wall riding or other checks
	if (MovementComponent->IsGrinding() || MovementComponent->IsGrounded()
		|| MovementComponent->IsDodging())
	{
		return false;
	}

	return true;
}

FGameplayTag& UPlayerStuntComponent::GetStuntTag()
{
	return StuntTag;
}

void UPlayerStuntComponent::AddStuntStruct(FStuntStruct& stuntStruct)
{
	if (CurrentStunts.Num() > 0 && stuntStruct.contextActor != nullptr)
	{
		if (CurrentStunts[CurrentStunts.Num()-1].contextActor == stuntStruct.contextActor)
		{
			return;
		}
	}
	if (CurrentStunts.Num() > 0 && CurrentStunts[CurrentStunts.Num()-1].StuntName == stuntStruct.StuntName)
	{
		stuntStruct.ScoreAddition = CurrentStunts[CurrentStunts.Num()-1].ScoreAddition * 0.5f;
		if (stuntStruct.ScoreAddition <= 50)
		{
			return;
		}
	}
	CurrentStunts.Add(stuntStruct);
	CurrentTimer = 0.f;
	if (LiquidPlayer->EnergyComponent->HasEnergy(LiquidPlayer->EnergyComponent->GetMaxEnergy() - 10))
	{
		LiquidPlayer->HealthComponent->Heal(stuntStruct.energyAddition/2);
	}
	LiquidPlayer->EnergyComponent->GainEnergy(stuntStruct.energyAddition);

	SetStuntString();
	LiquidPlayer->FeedbackComponent->OnStuntAdded.Broadcast();
}

void UPlayerStuntComponent::FailStunt()
{
	CurrentStunts.Empty();
	ResetStuntString();
	StuntString = "Stunt Failed";
	LiquidPlayer->EnergyComponent->LoseEnergy(20.f);
	CurrentTimer = GetTimePerStunt();
	OnStuntTextAdded.Broadcast();
	LiquidPlayer->FeedbackComponent->OnStuntFailed.Broadcast();
}

void UPlayerStuntComponent::ResetStuntString()
{
	StuntString = "";
}


bool UPlayerStuntComponent::CanStunt() const
{
	if (!IsValid(LiquidPlayer))
		return false;

	ULiquidMovementComponent* MovementComponent = LiquidPlayer->LiquidMovementComponent;

	//TODO: Stop Stunting if wall riding or other checks
	if (MovementComponent->IsGrinding() || MovementComponent->IsGrounded()
		|| MovementComponent->IsDodging() && !IsStunting)
	{
		return false;
	}

	return true;
}

void UPlayerStuntComponent::AddScore(int score)
{
	StuntScore += score;
	OnScoreAdded.Broadcast();
	LiquidPlayer->FeedbackComponent->OnStuntSuccessful.Broadcast();
	ULiquidGameInstance* gameInstance = Cast<ULiquidGameInstance>(GetWorld()->GetGameInstance());
	if(IsValid(gameInstance))
	{
		gameInstance->Score = StuntScore;
	}
}

