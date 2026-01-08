#include "BouncePadComponent.h"

#include "PlayerComponents/LiquidMovementComponent.h"
#include "Components/SphereComponent.h"
#include "LiquidGravity_Dev/Core/LiquidPlayer.h"

class ALiquidPlayer;

UBouncePadComponent::UBouncePadComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	OnComponentBeginOverlap.AddDynamic(this, &UBouncePadComponent::OnBeginOverlap);
}


void UBouncePadComponent::BeginPlay()
{
	ImpulseDirection.Normalize();
	Super::BeginPlay();
}

void UBouncePadComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UBouncePadComponent::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(OtherActor->IsA<ALiquidPlayer>())
	{
		// Cast the actor to the player
		ALiquidPlayer* LiquidPlayer = Cast<ALiquidPlayer>(OtherActor);
		if(LiquidPlayer)
		{
			if (!LiquidPlayer->DefaultOverlappingSphere->IsOverlappingActor(GetOwner()))
			{
				return;
			}
			LiquidPlayer->LiquidMovementComponent->SetBouncePad(this);
			LiquidPlayer->LiquidMovementComponent->AddBouncePadTag();
		}
	}
}