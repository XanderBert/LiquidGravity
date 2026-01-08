#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LiquidCamera.generated.h"

struct FuzzySelectStruct;
struct FInputActionValue;
class ALiquidPlayer;
class ULiquidGameInstance;

UCLASS()
class LIQUIDGRAVITY_DEV_API ALiquidCamera : public AActor
{
	GENERATED_BODY()

public:
	ALiquidCamera();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;


	//-------------------- Input --------------------
	void RotateCamera(const FInputActionValue& Value);
	void RotateCameraLockOn(FVector2D Value);

	//-------------------- Components --------------------
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UCameraComponent* CameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USpringArmComponent* SpringArmComponent;
	
	float TimeSinceRotationChange;


	//-------------------- Fuzzy Select --------------------
	//const FHitResult* GetFuzzySelectedClass(UClass* subClass, bool isInterface = false) const;
	//const FHitResult* GetFuzzySelectedComponent(UClass* subClass, bool isInterface = false) const;

	//const FHitResult* GetFuzzySelectedClass(FuzzySelectStruct fuzzyStruct) const;
	//const FHitResult* GetFuzzySelectedComponent(FuzzySelectStruct fuzzyStruct) const;

	const FHitResult* GetFuzzySelection(const FuzzySelectStruct& fuzzyStruct) const;

	UFUNCTION(BlueprintPure, Category = "UI Math")
	static bool ProjectWorldToScreenBidirectional(APlayerController const* Player, const FVector& WorldPosition, FVector2D& ScreenPosition, bool& bTargetBehindCamera, bool bPlayerViewportRelative = false);
	
private:
	FVector2D OrbitAngles{};
	FVector2D RotationChange{};
	FQuat GravityAlignment = FQuat::Identity;
	
	bool bNeedsUpdate = false;
	
	UPROPERTY(EditAnywhere, Category = Rotation)
	float RotationSpeed = 90.f;
		
	UPROPERTY(EditAnywhere, Category = Rotation)
	float ClampOverY = 75.f;;

	UPROPERTY(EditAnywhere, Category = Movement)
	float MovementExponent = 0.2f;

	UPROPERTY(EditAnywhere, Category = cAMERA)
	FVector CameraHeightLowMedHigh = FVector(80.f,120.f,160.f);

	UPROPERTY(EditAnywhere, Category = cAMERA)
	FVector CameraDistanceLowMedHigh = FVector(450.f,500.f,600.f);

	UPROPERTY()
	ULiquidGameInstance* LiquidGameInstance;

	UPROPERTY()
	ALiquidPlayer* LiquidPlayer;

	//-------------------- Fuzzy Select --------------------
	void FuzzySelect();

	UPROPERTY(EditAnywhere, Category = "Fuzzy Select")
	float FuzzySelectRadius = 10000.f;

	UPROPERTY(EditAnywhere, Category = "Fuzzy Select")
	float FuzzySelectLength = 10000.f;
	
	UPROPERTY()
	TArray<FHitResult> FuzzySelectHits;
};

struct FuzzySelectStruct
{
public:
	enum FuzzySelectType
	{
		Class,
		Component
	};
	
	float MinimumDistance = 0.f;
	float MaximumDistance = UE_BIG_NUMBER;
	//Default value: 0.5f, 0.5f (edges of screen)
	FVector2D MaximumViewportDistance = FVector2D(0.5f,0.5f);
	bool RaycastCheck = true;
	UClass* SubClass;
	bool isInterface = false;
	FuzzySelectType SelectionType = Class;
};
