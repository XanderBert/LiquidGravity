// Fill out your copyright notice in the Description page of Project Settings.


#include "Monument.h"


// Sets default values
AMonument::AMonument()
{
	PrimaryActorTick.bCanEverTick = true;

	Ring1Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Ring1Mesh"));
	RootComponent = Ring1Mesh;

	Ring2Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Ring2Mesh"));
	Ring2Mesh->SetupAttachment(Ring1Mesh);

	Ring3Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Ring3Mesh"));
	Ring3Mesh->SetupAttachment(Ring2Mesh);
}


// Called every frame
void AMonument::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FScopedMovementUpdate ScopedMovement(GetRootComponent(), EScopedUpdate::DeferredUpdates);

	// Get current rotations
	FRotator Rotation1 = Ring1Mesh->GetRelativeRotation();
	FRotator Rotation2 = Ring2Mesh->GetRelativeRotation();
	FRotator Rotation3 = Ring3Mesh->GetRelativeRotation();

	// Update rotations directly

	Rotation1.Pitch += Ring1Speed * DeltaTime;
	Rotation1.Yaw += Ring1Speed * DeltaTime * 0.4f;
	
	Rotation2.Roll += Ring2Speed * DeltaTime;
	Rotation2.Pitch += Ring2Speed * DeltaTime * 0.6f;

	
	Rotation3.Roll += Ring3Speed * DeltaTime;
	Rotation3.Yaw += Ring3Speed * DeltaTime * 0.9f;

	// Set new rotations
	Ring1Mesh->SetRelativeRotation(Rotation1);
	Ring2Mesh->SetRelativeRotation(Rotation2);
	Ring3Mesh->SetRelativeRotation(Rotation3);
}

