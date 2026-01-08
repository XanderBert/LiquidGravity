// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthPickup.h"

#include "NiagaraComponent.h"
#include "Components/SphereComponent.h"
#include "LiquidGravity_Dev/Components/HealthComponent.h"
#include "LiquidGravity_Dev/Core/LiquidPlayer.h"


// Sets default values
AHealthPickup::AHealthPickup()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CollectionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollectionSphere"));
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(CollectionSphere);
	Mesh->SetupAttachment(CollectionSphere);
}

void AHealthPickup::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	CurrentTime += DeltaSeconds;
	if (CurrentTime >= TimeToRespawn)
	{
		UNiagaraComponent* niagaraComponent = GetComponentByClass<UNiagaraComponent>();
		if (IsValid(niagaraComponent))
		{
			if (!niagaraComponent->IsActive())
				niagaraComponent->Activate(true);
		}
		CollectionSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}
}

void AHealthPickup::StartRespawn()
{
	CurrentTime = 0.f;
	UNiagaraComponent* niagaraComponent = GetComponentByClass<UNiagaraComponent>();
	if (IsValid(niagaraComponent))
	{
		niagaraComponent->DeactivateImmediate();
	}
	CollectionSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AHealthPickup::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	FString str = OtherActor->GetName();
	//GEngine->AddOnScreenDebugMessage(-125, 5.f, FColor::Green, str);
	ALiquidPlayer* liquidPlayer = Cast<ALiquidPlayer>(OtherActor);
	if (IsValid(liquidPlayer) && liquidPlayer->HealthComponent->GetCurrentHealth() < liquidPlayer->HealthComponent->GetMaxHealth() - 10)
	{
		liquidPlayer->HealthComponent->Heal(HealthAmount);
		StartRespawn();
		OnPickedUp.Broadcast();
	}
}

// Called when the game starts or when spawned
void AHealthPickup::BeginPlay()
{
	Super::BeginPlay();
	CollectionSphere->OnComponentBeginOverlap.AddDynamic(this, &AHealthPickup::OnSphereOverlap);
	CurrentTime = TimeToRespawn;
}
