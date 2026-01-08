// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HealthPickup.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPickedUpSignature);

class USphereComponent;

UCLASS()
class LIQUIDGRAVITY_DEV_API AHealthPickup : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AHealthPickup();
	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	USphereComponent* CollectionSphere;

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite, Category = "Components")
	UMeshComponent* Mesh;

	UPROPERTY(BlueprintAssignable)
	FOnPickedUpSignature OnPickedUp{};

private:
	void StartRespawn();
	UFUNCTION()
	void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere)
	float HealthAmount = 50.f;

	UPROPERTY(EditAnywhere)
	float TimeToRespawn = 20.f;

	float CurrentTime{};
};
