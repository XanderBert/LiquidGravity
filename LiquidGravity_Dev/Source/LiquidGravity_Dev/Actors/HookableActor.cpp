// Fill out your copyright notice in the Description page of Project Settings.


#include "HookableActor.h"


// Sets default values
AHookableActor::AHookableActor()
{
	PrimaryActorTick.bCanEverTick = true;
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh Component"));
}

// Called when the game starts or when spawned
void AHookableActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AHookableActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

