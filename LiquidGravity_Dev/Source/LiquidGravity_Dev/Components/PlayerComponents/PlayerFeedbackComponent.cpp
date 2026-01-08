#include "PlayerFeedbackComponent.h"

#include "LiquidMovementComponent.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "LiquidGravity_Dev/Core/LiquidGameInstance.h"
#include "LiquidGravity_Dev/Core/LiquidPlayer.h"

UPlayerFeedbackComponent::UPlayerFeedbackComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void UPlayerFeedbackComponent::BeginPlay()
{
	Super::BeginPlay();

	LiquidPlayer = Cast<ALiquidPlayer>(GetOwner());
	LiquidPlayer->SkatingSound->Play();
	LiquidPlayer->GrindingSound->Play();
}

void UPlayerFeedbackComponent::TickComponent(float DeltaTime, enum ELevelTick TickType,
                                             FActorComponentTickFunction* ThisTickFunction)
{
	ULiquidGameInstance* gameInstance = Cast<ULiquidGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));

	if (IsValid(LiquidPlayer))
	{
		float skatingVolume = LiquidPlayer->LiquidMovementComponent->GetDisplayVelocity().Length() / LiquidPlayer->LiquidMovementComponent->GetMaxSpeed();

		if (LiquidPlayer->LiquidMovementComponent->GetDisplayVelocity().Length() < 50.f
			|| !LiquidPlayer->LiquidMovementComponent->IsGrounded())
		{
			skatingVolume = 0.f;
		}

		skatingVolume = FMath::Clamp(skatingVolume, 0.f, 1.f);

		LiquidPlayer->SkatingSound->SetVolumeMultiplier(FMath::Lerp(0.f,gameInstance->SoundVolume, skatingVolume));


		FString soundVol = FString::SanitizeFloat(LiquidPlayer->SkatingSound->VolumeMultiplier);

		if (LiquidPlayer->LiquidMovementComponent->IsGrinding())
		{
			skatingVolume = LiquidPlayer->LiquidMovementComponent->GetDisplayVelocity().Length() / LiquidPlayer->LiquidMovementComponent->GetMaxSpeed();
			skatingVolume = FMath::Clamp(skatingVolume, 0.f, 1.f);
			
			LiquidPlayer->GrindingSound->SetVolumeMultiplier(FMath::Lerp(0.f,gameInstance->SoundVolume, skatingVolume));
		}
		else
		{
			LiquidPlayer->GrindingSound->SetVolumeMultiplier(0.f);	
		}
	}
}

void UPlayerFeedbackComponent::PlaySound(USoundBase* sound)
{
	ULiquidGameInstance* gameInstance = Cast<ULiquidGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	UGameplayStatics::PlaySound2D(GetWorld(),sound, gameInstance->SoundVolume,1.f,0.f);
}

void UPlayerFeedbackComponent::PlaySoundRandomPitch(USoundBase* sound, float minPitch, float maxPitch)
{
	ULiquidGameInstance* gameInstance = Cast<ULiquidGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	UGameplayStatics::PlaySoundAtLocation(GetWorld(),sound, GetOwner()->GetActorLocation(), GetOwner()->GetActorRotation(), gameInstance->SoundVolume,FMath::FRandRange(minPitch,maxPitch),0.f,gameInstance->SoundAttenuation);
}

void UPlayerFeedbackComponent::PlayForceFeedback(UForceFeedbackEffect* forceFeedback) const
{
	ULiquidGameInstance* gameInstance = Cast<ULiquidGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));

	if (gameInstance->VibrationEnabled)
	{
		//Play force feedback
		UGameplayStatics::GetPlayerController(GetWorld(), 0)->ClientPlayForceFeedback(forceFeedback);
	}
}

void UPlayerFeedbackComponent::PlayScreenShake(TSubclassOf<UCameraShakeBase> cameraShake) const
{
	ULiquidGameInstance* gameInstance = Cast<ULiquidGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));

	if (gameInstance->ScreenShakeEnabled)
		UGameplayStatics::GetPlayerController(GetWorld(), 0)->ClientStartCameraShake(cameraShake, gameInstance->ScreenShakeIntensity);
}
