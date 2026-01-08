// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HookableActor.generated.h"

UCLASS()
class LIQUIDGRAVITY_DEV_API AHookableActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AHookableActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(EditAnywhere, Category = "Components")
	class UStaticMeshComponent* MeshComponent;
};
