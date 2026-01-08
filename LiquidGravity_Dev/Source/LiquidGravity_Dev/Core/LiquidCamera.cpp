#include "LiquidCamera.h"

#include "LiquidGameInstance.h"
#include "LiquidPlayer.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "InputActionValue.h"
#include "LiquidMath.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "LiquidGravity_Dev/Components/PlayerComponents/CombatComponent.h"
#include "LiquidGravity_Dev/Components/PlayerComponents/LiquidMovementComponent.h"


// Sets default values
ALiquidCamera::ALiquidCamera()
{
	PrimaryActorTick.bCanEverTick = true;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));

	SetRootComponent(SpringArmComponent);
	CameraComponent->SetupAttachment(SpringArmComponent);
}


void ALiquidCamera::BeginPlay()
{
	Super::BeginPlay();
	LiquidGameInstance = Cast<ULiquidGameInstance>(GetGameInstance());
	LiquidPlayer = Cast<ALiquidPlayer>(GetOwner());
}


void ALiquidCamera::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FuzzySelect();

	FVector desiredTrans;

	float desiredTargetArmLength{};
	if (OrbitAngles.Y < 0)
	{
		float delta = FMath::Abs(OrbitAngles.Y/ClampOverY);
		desiredTrans = LiquidPlayer->GetActorLocation() + GetActorUpVector() * FMath::Lerp(CameraHeightLowMedHigh.Y,CameraHeightLowMedHigh.X, delta);
		desiredTargetArmLength = FMath::Lerp(CameraDistanceLowMedHigh.Y,CameraDistanceLowMedHigh.X, delta);
	}
	else
	{
		float delta = FMath::Abs(OrbitAngles.Y/ClampOverY);
		desiredTrans = LiquidPlayer->GetActorLocation() + GetActorUpVector() * FMath::Lerp(CameraHeightLowMedHigh.Y,CameraHeightLowMedHigh.Z, delta);
		desiredTargetArmLength = FMath::Lerp(CameraDistanceLowMedHigh.Y,CameraDistanceLowMedHigh.Z, delta);
	}

	SpringArmComponent->TargetArmLength = FLiquidMath::LerpSmooth(SpringArmComponent->TargetArmLength, desiredTargetArmLength, DeltaTime, 0.5f, 0.01f);

	if (LiquidPlayer->CombatComponent->GetIsLockingOn())
	{
		auto lockOnActor = LiquidPlayer->CombatComponent->GetLockedOnActor();
		if(!IsValid(lockOnActor))
		{
			LiquidPlayer->CombatComponent->SetIsLockingOn(false);
			return;
		}
		
		float lerpFactor = 0.2f;
		float distanceBehindPlayer = (lockOnActor->GetActorLocation() - LiquidPlayer->GetActorLocation()).Length();
		float distanceRightOfEnemy = 2000;
		FVector middlePosition =  FMath::Lerp(desiredTrans
			- GetActorForwardVector() * lerpFactor * distanceBehindPlayer,
			lockOnActor->GetActorLocation()
			+ lockOnActor->GetActorRightVector() * -distanceRightOfEnemy * lerpFactor,
			0.2f);
		
		desiredTrans = middlePosition;
	}
	
	
	//Lerp Follow the PlayerToFollow
	const FVector currentTrans = GetActorLocation();
	//const FVector newTrans = FMath::InterpEaseIn(currentTrans, desiredTrans, DeltaTime, MovementExponent);
	const FVector newTrans = FLiquidMath::LerpSmooth(currentTrans, desiredTrans, DeltaTime, 0.1f, 0.01f);
	
	if (LiquidPlayer->CombatComponent->GetIsLockingOn())
	{
		FVector2D screenSpaceSize = UWidgetLayoutLibrary::GetViewportSize(GetWorld());

		bool behindCamera;
		
		FVector2D enemyScreenSpacePosition;
		ProjectWorldToScreenBidirectional(GetWorld()->GetFirstPlayerController(),
			LiquidPlayer->CombatComponent->GetLockedOnActor()->GetActorLocation(),
			enemyScreenSpacePosition,behindCamera);

		if (behindCamera)
		{
			LiquidPlayer->CombatComponent->SetIsLockingOn(false);
		}
		else
		{
			FVector2D enemyViewportPosition = FVector2D(enemyScreenSpacePosition.X/screenSpaceSize.X,
				enemyScreenSpacePosition.Y/screenSpaceSize.Y);

			FVector2D deltaMovement = (FVector2D (0.5f,0.5f) - enemyViewportPosition) * FMath::Lerp(0.f,1.f,
				(FVector2D (0.5f,0.5f) - enemyViewportPosition).Length()/0.7f) *
					10000.f;

			//FString debugString = FString::SanitizeFloat(deltaMovement.X) + "/" + FString::SanitizeFloat(deltaMovement.Y);
			//GEngine->AddOnScreenDebugMessage(-512,0.1f, FColor::White, *debugString);

			RotateCameraLockOn(FVector2D(-deltaMovement.X,deltaMovement.Y) * DeltaTime);
		}
		//enemyScreenSpacePosition *= UWidgetLayoutLibrary::GetViewportScale(GetWorld());
	}
	
	if(bNeedsUpdate)
	{
		OrbitAngles += RotationChange * DeltaTime;
		
		//Clamp
		OrbitAngles.Y = FMath::Clamp(OrbitAngles.Y, -ClampOverY, ClampOverY);

		if (OrbitAngles.X > 360)
		{
			OrbitAngles.X-=360;
		}
		if (OrbitAngles.X < -360)
		{
			OrbitAngles.X+=360;
		}

		RotationChange = FVector2D::ZeroVector;
		bNeedsUpdate = false;
	}
	
	FRotator desiredRotation = FRotator(OrbitAngles.Y, OrbitAngles.X, 0);
	
	//Apply the gravity alignment to the desired rotation
	FQuat oldGravityAlignment = GravityAlignment;

	if(LiquidPlayer->LiquidMovementComponent->GetLockGravity())
	{
		GravityAlignment = LiquidPlayer->LiquidMovementComponent->GetLockGravityOrientation();
	}
	else
	{
		GravityAlignment = LiquidGameInstance->Gravity.GetGravityAlignment(LiquidPlayer->GetActorLocation(), GravityAlignment);
	}

	//Smooth the gravity alignment, don't smooth the Mouse Input
	//TODO: Make Time and Precision parameters
	FQuat smoothedGravityAlignment = FLiquidMath::SlerpSmooth(oldGravityAlignment, GravityAlignment, DeltaTime, 1.3f, 0.01f);

	//Update gravity alignment so it reflects the change
	GravityAlignment = smoothedGravityAlignment;
	
	FQuat lookRotation = smoothedGravityAlignment * desiredRotation.Quaternion();
	FRotator lookRotationRotator = lookRotation.Rotator();
	
	SetActorLocationAndRotation(newTrans, lookRotationRotator);
	/*
	OrbitAngles.X = GetActorRotation().Yaw;
	OrbitAngles.Y = GetActorRotation().Pitch;
	OrbitAngles.Z = GetActorRotation().Roll;*/
}

void ALiquidCamera::RotateCamera(const FInputActionValue& Value)
{
	FVector2D mouseInput = Value.Get<FVector2D>();

	ULiquidGameInstance* gameInstance = Cast<ULiquidGameInstance>(GetGameInstance());
	if (IsValid(gameInstance))
	{
		mouseInput *= gameInstance->CameraSensitivity;
	}

	if (LiquidPlayer->CombatComponent->GetIsLockingOn())
	{
		mouseInput *= 0.25f;
	}
	//Check if the mouse is moving more than a small number
	if (mouseInput.X < -UE_KINDA_SMALL_NUMBER || mouseInput.X > UE_KINDA_SMALL_NUMBER || mouseInput.Y < -UE_KINDA_SMALL_NUMBER || mouseInput.Y > UE_KINDA_SMALL_NUMBER)
	{
		bNeedsUpdate = true;
		RotationChange = +RotationSpeed * mouseInput;
	}
}

void ALiquidCamera::RotateCameraLockOn(const FVector2D Value)
{
	FVector2D mouseInput = Value;

	//Check if the mouse is moving more than a small number
	if (mouseInput.X < -UE_KINDA_SMALL_NUMBER || mouseInput.X > UE_KINDA_SMALL_NUMBER || mouseInput.Y < -UE_KINDA_SMALL_NUMBER || mouseInput.Y > UE_KINDA_SMALL_NUMBER)
	{
		bNeedsUpdate = true;
		RotationChange += RotationSpeed * mouseInput;
	}
}


/*

const FHitResult* ALiquidCamera::GetFuzzySelectedClass(FuzzySelectStruct fuzzyStruct) const
{
	if(FuzzySelectHits.Num() == 0)
	{
		return nullptr;
	}
	
	FVector2D screenSpaceSize = GEngine->GameViewport->Viewport->GetSizeXY();
	FVector2D halfScreenSpaceSize = screenSpaceSize / 2;
	
	FVector2D hitClosestToCentre{UE_BIG_NUMBER, UE_BIG_NUMBER};
	FVector2D currentScreenSpace{};
	int indexOfObjectClosestToCentre{};
	
	UGameplayStatics::ProjectWorldToScreen(GetWorld()->GetFirstPlayerController(), FuzzySelectHits[0].Location, hitClosestToCentre);
}


const FHitResult* ALiquidCamera::GetFuzzySelectedComponent(FuzzySelectStruct fuzzyStruct) const
{
	if(FuzzySelectHits.Num() == 0) return nullptr;
	
	FVector2D screenSpaceSize = GEngine->GameViewport->Viewport->GetSizeXY();
	FVector2D halfScreenSpaceSize = screenSpaceSize / 2;
	
	FVector2D hitClosestToCentre{UE_BIG_NUMBER, UE_BIG_NUMBER};
	FVector2D currentScreenSpace{};
	int indexOfObjectClosestToCentre{};
	
	UGameplayStatics::ProjectWorldToScreen(GetWorld()->GetFirstPlayerController(), FuzzySelectHits[0].Location, hitClosestToCentre);

	for (int i{}; i < FuzzySelectHits.Num() ; ++i)
	{
		if(!IsValid(FuzzySelectHits[i].GetActor())) continue;
		
		auto componentsOfActor = FuzzySelectHits[i].GetActor()->GetComponents();

		//Go over components
		for(auto it = componentsOfActor.CreateIterator(); it; ++it)
		{
			if(fuzzyStruct.isInterface)
			{
				if(!(*it)->GetClass()->ImplementsInterface(fuzzyStruct.SubClass)) continue;
			}
			else
			{
				if(!(*it)->IsA(fuzzyStruct.SubClass)) continue;
			}
			
			UGameplayStatics::ProjectWorldToScreen(GetWorld()->GetFirstPlayerController(), FuzzySelectHits[i].Location, currentScreenSpace);
			
			if(FVector2D::Distance(currentScreenSpace, halfScreenSpaceSize) < FVector2D::Distance(hitClosestToCentre, halfScreenSpaceSize))
			{
				hitClosestToCentre = currentScreenSpace;
				indexOfObjectClosestToCentre = i;
			}
		}

		
		UGameplayStatics::ProjectWorldToScreen(GetWorld()->GetFirstPlayerController(), FuzzySelectHits[i].Location, currentScreenSpace);
		
		if(FVector2D::Distance(currentScreenSpace, halfScreenSpaceSize) < FVector2D::Distance(hitClosestToCentre, halfScreenSpaceSize))
		{
			hitClosestToCentre = currentScreenSpace;
			indexOfObjectClosestToCentre = i;
		}
	}
	
	return &FuzzySelectHits[indexOfObjectClosestToCentre];
}
*/
const FHitResult* ALiquidCamera::GetFuzzySelection(const FuzzySelectStruct& fuzzyStruct) const
{
	if(FuzzySelectHits.Num() == 0) return nullptr;
	
	FVector2D screenSpaceSize = UWidgetLayoutLibrary::GetViewportSize(GetWorld());

	//FString loc = FString::SanitizeFloat(screenSpaceSize.X) + "/" + FString::SanitizeFloat(screenSpaceSize.Y);
	//GEngine->AddOnScreenDebugMessage(-125,0.1f,FColor::Red,*loc);
	
	FVector2D halfScreenSpaceSize = screenSpaceSize / 2;
	
	FVector2D hitClosestToCentre{fuzzyStruct.MaximumViewportDistance.X * screenSpaceSize.X + halfScreenSpaceSize.X, fuzzyStruct.MaximumViewportDistance.X * screenSpaceSize.Y + halfScreenSpaceSize.Y};
	FVector2D currentScreenSpace{};
	int indexOfObjectClosestToCentre{};
	
	bool temp;
	ProjectWorldToScreenBidirectional(GetWorld()->GetFirstPlayerController(), FuzzySelectHits[0].ImpactPoint, currentScreenSpace,temp);
	//currentScreenSpace *= UWidgetLayoutLibrary::GetViewportScale(GetWorld());

	if (fuzzyStruct.SelectionType == FuzzySelectStruct::Component)
	{
		bool flag = false;
		
		for (int i{}; i < FuzzySelectHits.Num() ; ++i)
		{
			if(!IsValid(FuzzySelectHits[i].GetActor())) continue;
		
			auto componentsOfActor = FuzzySelectHits[i].GetActor()->GetComponents();

			//Go over components
			for(auto it = componentsOfActor.CreateIterator(); it; ++it)
			{
				if(fuzzyStruct.isInterface)
				{
					if(!(*it)->GetClass()->ImplementsInterface(fuzzyStruct.SubClass)) continue;
				}
				else
				{
					if(!(*it)->IsA(fuzzyStruct.SubClass)) continue;
				}

				bool behindCamera;
				
				ProjectWorldToScreenBidirectional(GetWorld()->GetFirstPlayerController(), FuzzySelectHits[i].ImpactPoint, currentScreenSpace,behindCamera);

				if (behindCamera)
				{
					continue;;
				}
				
				//currentScreenSpace *= UWidgetLayoutLibrary::GetViewportScale(GetWorld());

				if (FVector::Distance(GetActorLocation(), FuzzySelectHits[i].ImpactPoint) < fuzzyStruct.MinimumDistance)
				{
					continue;
				}

				if (FVector::Distance(GetActorLocation(), FuzzySelectHits[i].ImpactPoint) > fuzzyStruct.MaximumDistance)
				{
					continue;
				}

				if (fuzzyStruct.RaycastCheck)
				{
					FCollisionQueryParams params;
					params.AddIgnoredActor(FuzzySelectHits[i].GetActor());
					params.AddIgnoredActor(this);
					FHitResult hit;
					GetWorld()->LineTraceSingleByChannel(hit, FuzzySelectHits[i].GetActor()->GetActorLocation(),
							LiquidPlayer->GetActorLocation(),ECC_Visibility,params);

					if (Cast<ALiquidPlayer>(hit.GetActor()) != LiquidPlayer)
					{
						continue;
					}
				}
				
				if(FVector2D::Distance(currentScreenSpace, halfScreenSpaceSize) < FVector2D::Distance(hitClosestToCentre, halfScreenSpaceSize))
				{
					hitClosestToCentre = currentScreenSpace;
					indexOfObjectClosestToCentre = i;
					flag = true;
				}
			}
		}
		
		if (flag)
			return &FuzzySelectHits[indexOfObjectClosestToCentre];
		else
		{
			return nullptr;
		}
	}

	else
	{
		bool flag = false;

		FString newLoc;
		
		for (int i{}; i < FuzzySelectHits.Num() ; ++i)
		{
			if(!IsValid(FuzzySelectHits[i].GetActor())) continue;
		
			if(fuzzyStruct.isInterface)
			{
				if(!FuzzySelectHits[i].GetActor()->GetClass()->ImplementsInterface(fuzzyStruct.SubClass)) continue;
			}
			else
			{
				if(!FuzzySelectHits[i].GetActor()->IsA(fuzzyStruct.SubClass)) continue;
			}
			
			bool behindCamera;
			ProjectWorldToScreenBidirectional(GetWorld()->GetFirstPlayerController(), FuzzySelectHits[i].ImpactPoint, currentScreenSpace,behindCamera);

			if(behindCamera)
			{
				continue;
			}

			//currentScreenSpace *= UWidgetLayoutLibrary::GetViewportScale(GetWorld());

			if (FVector::Distance(GetActorLocation(), FuzzySelectHits[i].ImpactPoint) < fuzzyStruct.MinimumDistance)
			{
				continue;
			}

			if (FVector::Distance(GetActorLocation(), FuzzySelectHits[i].ImpactPoint) > fuzzyStruct.MaximumDistance)
			{
				continue;
			}

			if (fuzzyStruct.RaycastCheck)
			{
				if (fuzzyStruct.RaycastCheck)
				{
					FCollisionQueryParams params;
					params.AddIgnoredActor(FuzzySelectHits[i].GetActor());
					params.AddIgnoredActor(this);
					FHitResult hit;
					GetWorld()->LineTraceSingleByChannel(hit, FuzzySelectHits[i].GetActor()->GetActorLocation(),
						LiquidPlayer->GetActorLocation(),ECC_Visibility,params);

					if (Cast<ALiquidPlayer>(hit.GetActor()) != LiquidPlayer)
					{
						continue;
					}
				}
			}
			//UGameplayStatics::ProjectWorldToScreen(GetWorld()->GetFirstPlayerController(), FuzzySelectHits[i].Location, currentScreenSpace);

			newLoc += FuzzySelectHits[i].GetActor()->GetName() + " -> " + FString::SanitizeFloat(currentScreenSpace.X) + "/" + FString::SanitizeFloat(currentScreenSpace.Y) + "\n";
			
			if(FVector2D::Distance(currentScreenSpace, halfScreenSpaceSize) < FVector2D::Distance(hitClosestToCentre, halfScreenSpaceSize))
			{
				flag = true;
				hitClosestToCentre = currentScreenSpace;
				indexOfObjectClosestToCentre = i;
			}
		}
		//GEngine->AddOnScreenDebugMessage(-126,0.1f,FColor::Red,*newLoc);

		if (flag)
		{
			//GEngine->AddOnScreenDebugMessage(-152512,0.1f,FColor::White, FuzzySelectHits[indexOfObjectClosestToCentre].GetActor()->GetName());
			return &FuzzySelectHits[indexOfObjectClosestToCentre];
		}
		return nullptr;
	}
}

bool ALiquidCamera::ProjectWorldToScreenBidirectional(APlayerController const* Player, const FVector& WorldPosition, FVector2D& ScreenPosition, bool& bTargetBehindCamera, bool bPlayerViewportRelative)
{
	FVector Projected;
	bool bSuccess = false;

	ULocalPlayer* const LP = Player ? Player->GetLocalPlayer() : nullptr;
	if (LP && LP->ViewportClient)
	{
		// get the projection data
		FSceneViewProjectionData ProjectionData;
		if (LP->GetProjectionData(LP->ViewportClient->Viewport, /*out*/ ProjectionData))
		{
			const FMatrix ViewProjectionMatrix = ProjectionData.ComputeViewProjectionMatrix();
			const FIntRect ViewRectangle = ProjectionData.GetConstrainedViewRect();

			FPlane Result = ViewProjectionMatrix.TransformFVector4(FVector4(WorldPosition, 1.f));
			if (Result.W < 0.f) { bTargetBehindCamera = true; } else { bTargetBehindCamera = false; }
			if (Result.W == 0.f) { Result.W = 1.f; } // Prevent Divide By Zero

			const float RHW = 1.f / FMath::Abs(Result.W);
			Projected = FVector(Result.X, Result.Y, Result.Z) * RHW;

			// Normalize to 0..1 UI Space
			const float NormX = (Projected.X / 2.f) + 0.5f;
			const float NormY = 1.f - (Projected.Y / 2.f) - 0.5f;

			Projected.X = (float)ViewRectangle.Min.X + (NormX * (float)ViewRectangle.Width());
			Projected.Y = (float)ViewRectangle.Min.Y + (NormY * (float)ViewRectangle.Height());

			bSuccess = true;
			ScreenPosition = FVector2D(Projected.X, Projected.Y);

			if (bPlayerViewportRelative)
			{
				ScreenPosition -= FVector2D(ProjectionData.GetConstrainedViewRect().Min);
			}
		}
		else
		{
			ScreenPosition = FVector2D(1234, 5678);
		}
	}

	return bSuccess;
}

void ALiquidCamera::FuzzySelect()
{
	FVector forward = GetActorForwardVector();
	FVector location = GetActorLocation();
	
	
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this);
	CollisionParams.AddIgnoredActor(LiquidPlayer);
	GetWorld()->SweepMultiByChannel(FuzzySelectHits, location, location - FuzzySelectRadius * forward + forward * FuzzySelectLength, FQuat::Identity, ECC_Camera, FCollisionShape::MakeSphere(FuzzySelectRadius), CollisionParams);
	
	 if (FuzzySelectHits.Num() > 0)
	 {
	 	//FVector2D screenSize = UWidgetLayoutLibrary::GetViewportSize(GetWorld());
	
	 	
	 	for (int i = FuzzySelectHits.Num()-1; i >= 0; i--)
	 	{
	 		//Discard hits if close to camera
	 		if (FVector::Distance(GetActorLocation(),LiquidPlayer->GetActorLocation()) - 100.f >
	 			FVector::Distance(GetActorLocation(), FuzzySelectHits[i].ImpactPoint))
	 		{
	 			FuzzySelectHits.RemoveAt(i);
	 		}
	 	}
	 }
}

