#include "ThrowableActor.h"

#include "Components/CapsuleComponent.h"
#include "Enemies/BaseEnemy.h"
#include "Kismet/GameplayStatics.h"
#include "LiquidGravity_Dev/Components/HealthComponent.h"
#include "LiquidGravity_Dev/Components/PlayerComponents/PlayerStuntComponent.h"

AThrowableActor::AThrowableActor()
{
	PrimaryActorTick.bCanEverTick = true;
	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComponent"));
	CapsuleComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CapsuleComponent->SetCollisionResponseToAllChannels(ECR_Overlap);
	CapsuleComponent->OnComponentBeginOverlap.AddDynamic(this, &AThrowableActor::OnCapsuleOverlapBegin);
	SetRootComponent(CapsuleComponent);
	
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	StaticMeshComponent->SetupAttachment(CapsuleComponent);
}


void AThrowableActor::SetThrown(const FVector& Force)
{
	CapsuleComponent->SetSimulatePhysics(true);
	CapsuleComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);


	//TODO: Remove this once we use a socket
	auto currentPos = GetActorLocation();
	auto direction = Force.GetSafeNormal();
	auto endPos = currentPos + direction * 100.f;
	SetActorLocation(endPos);
	
	CapsuleComponent->AddForce(Force * 10);

	bIsThrown = true;
}

void AThrowableActor::SetPull() const
{
	if (!IsValid(this))
		return;
	CapsuleComponent->SetSimulatePhysics(false);
	CapsuleComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}


void AThrowableActor::OnCapsuleOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                            UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(!bIsThrown) return;

	
	if(OtherActor->IsA<ABaseEnemy>())
	{
		auto enemy = Cast<ABaseEnemy>(OtherActor);
		if (enemy->HealthComponent->GetCurrentHealth() < DamageOnThrow)
		{
			enemy->bSpawnDeadBody = false;
			enemy->OnDeadOnThrow.Broadcast();
		}
		enemy->HealthComponent->TakeDamage(DamageOnThrow, UGameplayStatics::GetPlayerPawn(GetWorld(),0),
			UGameplayStatics::GetPlayerPawn(GetWorld(),0)->GetActorLocation());
		
		FStuntStruct stuntStruct;
		stuntStruct.StuntName = "Object thrown";
		Destroy();
		return;
	}

	
	//Start a timer to reset the throw
	GetWorldTimerManager().SetTimer(ThrowTimerHandle, this, &AThrowableActor::ResetThrow, TimeToResetThrow, false);
}

void AThrowableActor::ResetThrow()
{
	bIsThrown = false;
}
