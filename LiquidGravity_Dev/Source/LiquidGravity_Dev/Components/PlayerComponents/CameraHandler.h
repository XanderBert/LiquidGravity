#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/TimelineComponent.h"
#include "CameraHandler.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLiquidCameraActive);

class ALiquidCamera;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LIQUIDGRAVITY_DEV_API UCameraHandler : public UActorComponent
{
	GENERATED_BODY()
public:
	
	UCameraHandler();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,FActorComponentTickFunction* ThisTickFunction) override;
	
	UFUNCTION(BlueprintCallable)
	void TransitionToMainCamera();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	ALiquidCamera* LiquidCamera;

	FOnLiquidCameraActive OnLiquidCameraActived;
private:
	//-------------------- Camera --------------------
	UPROPERTY(EditDefaultsOnly, Category = Camera)
	TSubclassOf<ALiquidCamera> CameraClass;

	UPROPERTY(EditAnywhere, Category = Camera)
	TSoftObjectPtr<ACameraActor> TVCamera;

	FVector StartingCameraLocation;
	FRotator StartingCameraRotation;
	
	//Timeline for the transition from TVCamera to LiquidCamera
	UPROPERTY(EditAnywhere, Category = Camera)
	FTimeline CameraTimeline;

	UPROPERTY(EditAnywhere, Category = Camera)
	UCurveFloat* CameraCurve;

	FOnTimelineFloat OnTimelineUpdate;

	UFUNCTION()
	void TransitionToLiquidCamera(float alpha);

	UFUNCTION()
	void SetPlayerCameraActive();
	
	void SpawnCamera();
};
