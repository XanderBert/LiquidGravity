#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Monument.generated.h"

UCLASS()
class LIQUIDGRAVITY_DEV_API AMonument : public AActor
{
	GENERATED_BODY()

public:
	AMonument();
	virtual void Tick(float DeltaTime) override;
	

	UPROPERTY(EditAnywhere, Category = "Components")
	UStaticMeshComponent* Ring1Mesh;

	UPROPERTY(EditAnywhere, Category = "Components")
	UStaticMeshComponent* Ring2Mesh;

	UPROPERTY(EditAnywhere, Category = "Components")
	UStaticMeshComponent* Ring3Mesh;

	
	UPROPERTY(EditAnywhere)
	float Ring1Speed = 30.0f;

	UPROPERTY(EditAnywhere)
	float Ring2Speed = 30.0f;

	UPROPERTY(EditAnywhere)
	float Ring3Speed = 30.0f;
};