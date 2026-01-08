#include "ChangeGravityComponent.h"

#include "CameraHandler.h"
#include "LiquidMath.h"
#include "LiquidMovementComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "LiquidGravity_Dev/Core/LiquidCamera.h"
#include "LiquidGravity_Dev/Core/LiquidGameInstance.h"
#include "LiquidGravity_Dev/Core/LiquidPlayer.h"
#include "LiquidGravity_Dev/Interfaces/GravityInterface.h"


UChangeGravityComponent::UChangeGravityComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UChangeGravityComponent::BeginPlay()
{
	Super::BeginPlay();
	LiquidPlayer = Cast<ALiquidPlayer>(GetOwner());
	check(LiquidPlayer);

	if (GravityDirectionFX)
	{
		// Spawn the Niagara system at the same location as the bullet
		GravityDirectionNiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			GravityDirectionFX,
			GetOwner()->GetActorLocation(),
			GetOwner()->GetActorRotation() 
		);
	}

	if (GravityDirectionNiagaraComponent)
	{
		GravityDirectionNiagaraComponent->SetVectorParameter(TEXT("EndPos"), GetOwner()->GetActorLocation());

		GravityDirectionNiagaraComponent->SetVectorParameter(TEXT("StartPos"), GetOwner()->GetActorLocation());
	}

	LiquidGameInstance = Cast<ULiquidGameInstance>(GetWorld()->GetGameInstance());
}

void UChangeGravityComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	//This is to prevent calling pure virtual functions when a level is reloading
	LiquidGameInstance->Gravity.OnGameEnd();
}

void UChangeGravityComponent::HandleGravityFX(float DeltaTime)
{
	if (!GravityDirectionNiagaraComponent)
	{
		return;
	}

	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(LiquidPlayer);

	FHitResult hit;
	if (GetWorld()->SweepSingleByChannel(hit, LiquidPlayer->GetActorLocation(),
		LiquidPlayer->GetActorLocation() + LiquidPlayer->LiquidMovementComponent->GetGravityAtPlayerPosition().GetSafeNormal() * 1000,
		FQuat::Identity, ECC_Camera, FCollisionShape::MakeSphere(100),CollisionParams))
	{
		GravityDirectionEndPosition = FLiquidMath::LerpSmooth(GravityDirectionEndPosition,
			hit.ImpactPoint, DeltaTime,
			1.f,0.01f);
	}
	else
	{
		GravityDirectionEndPosition = FLiquidMath::LerpSmooth(GravityDirectionEndPosition,
			LiquidPlayer->LiquidMovementComponent->GetGravityAtPlayerPosition().GetSafeNormal() * 1000,
			DeltaTime,1.f,0.01f);
	}

	GravityDirectionNiagaraComponent->SetVectorParameter(TEXT("EndPos"), GravityDirectionEndPosition);

	GravityDirectionNiagaraComponent->SetVectorParameter(TEXT("StartPos"), GetOwner()->GetActorLocation());
}

bool UChangeGravityComponent::IsValidSurface(IGravityInterface* currentHit, const FHitResult* hit) const
{
	// If Gravity is in same direction as current direction, no point in changing
	// SUPER LAZY HACK?! REWORK IT???
	if (currentHit->GetGravity(CurrentHitLocation).GetSafeNormal().Dot(LiquidPlayer->LiquidMovementComponent->GetGravityAtPlayerPosition().GetSafeNormal()) > 0.75f)
		return false;
	const float distance = FVector::Distance(hit->ImpactPoint, LiquidPlayer->GetActorLocation());
	if(LiquidGameInstance->Gravity.GetGravitySource() != currentHit && distance < MaximumDistance)
		return true;

	if (LiquidGameInstance->Gravity.GetGravitySource() == currentHit
						&& ((LiquidPlayer->LiquidMovementComponent->GetLockGravity()) || LiquidGameInstance->Gravity.GetIsForcedGravity()
							|| (distance > MinimumDistance && distance < MaximumDistance)))
		return true;
	

	return false;
}


void UChangeGravityComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if(!LiquidGameInstance) return;
	if(!LiquidPlayer) return;

	HandleGravityFX(DeltaTime);

	FuzzySelectStruct fuzzySelectStruct = FuzzySelectStruct();
	fuzzySelectStruct.SelectionType = FuzzySelectStruct::Component;
	fuzzySelectStruct.isInterface = true;
	fuzzySelectStruct.RaycastCheck = false;
	fuzzySelectStruct.SubClass = UGravityInterface::StaticClass();
	const FHitResult* hit = LiquidPlayer->CameraHandler->LiquidCamera->GetFuzzySelection(fuzzySelectStruct);
	if (hit == nullptr)
	{
		SetVisibility(false);
		CurrentGravityField = nullptr;
		currentGravFieldActor = nullptr;
		CurrentHitLocation = LiquidPlayer->GetActorLocation();
	}
	else
	{
		if(hit->Component.IsValid())
		{
			IGravityInterface* currentHit = Cast<IGravityInterface>(hit->Component);
			if (currentHit == nullptr)
			{
				SetVisibility(false);
				return;
			}
			
			if (currentHit->IsComposite)
			{
				currentHit = currentHit->CompositeGravityInterface;
			}

			//Lazy hack for isvalid check
			CurrentHitLocation = hit->ImpactPoint;
			//TODO: Implement ignoring of the CURRENT GRAVITY INTERFACE, all of them if its a composite interface
			/*
			if (IsValid(Cast<USceneComponent>(LiquidGameInstance->Gravity.GetGravitySource()->_getUObject())))
			{
				if(LiquidGameInstance->Gravity.GetGravitySource() == currentHit
					&& LiquidGameInstance->Gravity.GetGravitySource()->IsComposite)
				{
					SetVisibility(false);
					return;
				}
			}*/
		
			/*if((LiquidGameInstance->Gravity.GetGravitySource() != currentHit)
				|| (LiquidGameInstance->Gravity.GetGravitySource() == currentHit
					&& LiquidPlayer->LiquidMovementComponent->GetLockGravity()))*/
			//TODO: Distance Check
			if (IsValidSurface(currentHit, hit))
			{
				SetVisibility(true);

				//constexpr float meshOffset = 100;
				//FVector interpolatedLocation = FLiquidMath::LerpSmooth(GetComponentLocation(), hit.Location- hit.Normal* 100,DeltaTime,0.5f,0.01f);
				FVector interpolatedLocation = FLiquidMath::LerpSmooth(GetComponentLocation(), hit->ImpactPoint ,DeltaTime,0.5f,0.01f);
			
				// TODO: Take mesh normal instead of impact normal / turn this into a decal?
				
				SetWorldLocation(interpolatedLocation + hit->ImpactNormal.GetSafeNormal() * 10.f);
				//SetWorldLocation(hit.ImpactPoint);

				TArray<FHitResult> lineTraceRotationResults;
				FVector direction = (hit->Location - LiquidPlayer->CameraHandler->LiquidCamera->GetActorLocation()).GetSafeNormal();
				GetWorld()->LineTraceMultiByChannel(lineTraceRotationResults,
					LiquidPlayer->CameraHandler->LiquidCamera->GetActorLocation(),
					hit->Location + direction * 1000.f, ECC_Camera);
				//Rotate the mesh to be aligned with ht normal

				FRotator rotation;
				
				bool flag = false;
				for (FHitResult& result : lineTraceRotationResults)
				{
					if (result.GetActor() == hit->GetActor())
					{
						rotation = (result.Normal.Rotation());
						flag = true;
						break;
					}
				}
				if (!flag)
				{
					rotation = (hit->Normal.Rotation());
				}

				FQuat smoothedRotation = FLiquidMath::SlerpSmooth(GetComponentQuat(), rotation.Quaternion(), DeltaTime, 0.5f, 0.01f);
				SetWorldRotation(smoothedRotation);
			
				if(currentHit->IsComposite)
				{
					CurrentGravityField = currentHit->CompositeGravityInterface;
					currentGravFieldActor = hit->GetActor()->GetParentActor();
				}
				else
				{
					CurrentGravityField = currentHit;
					currentGravFieldActor = hit->GetActor();
				}
				
				TargetPosition = hit->ImpactPoint;
			}
			else
			{
				SetVisibility(false);
				CurrentGravityField = nullptr;
				currentGravFieldActor = nullptr;
			}
		}
		else
		{
			SetVisibility(false);
			CurrentGravityField = nullptr;
			currentGravFieldActor = nullptr;
		}
	}
}


void UChangeGravityComponent::SetLookedAtGravityFieldActive()
{ 
	if (!CurrentGravityField) return;
	CachedTargetPosition = TargetPosition;
	AddChangeGravityTag();
	//LiquidPlayer->LiquidMovementComponent->SetLockGravity(true);
	CurrentGravityField->Execute_SetGravityFieldActive(true,currentGravFieldActor);
	//SetVisibility(false);
}

void UChangeGravityComponent::AddChangeGravityTag() const
{
	LiquidPlayer->GameplayTags.AddTag(ChangeGravityTag);
}

void UChangeGravityComponent::RemoveChangeGravityTag() const
{
	LiquidPlayer->GameplayTags.RemoveTag(ChangeGravityTag);
}
