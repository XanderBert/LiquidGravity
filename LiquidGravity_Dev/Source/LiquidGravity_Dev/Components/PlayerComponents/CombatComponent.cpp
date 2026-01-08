#include "CombatComponent.h"

#include "CameraHandler.h"
#include "../EnergyComponent.h"
#include "../HealthComponent.h"

#include "LiquidGravity_Dev/Actors/Enemies/BaseEnemy.h"
#include "LiquidGravity_Dev/Core/LiquidCamera.h"
#include "LiquidGravity_Dev/Core/LiquidPlayer.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "PlayerFeedbackComponent.h"
#include "PlayerStuntComponent.h"
#include "Kismet/GameplayStatics.h"
#include "LiquidGravity_Dev/Actors/ThrowableActor.h"
#include "LiquidGravity_Dev/DataTables/EnergyConsumptionDataTable.h"


UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bTickEvenWhenPaused = false;
	
	SetWidgetSpace(EWidgetSpace::Screen);
}

void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	LiquidPlayer = Cast<ALiquidPlayer>(GetOwner());
	DisableWidget();
}


void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	// Only tick when we are in gameplay mode
	if (!GetWorld() || !GetWorld()->IsGameWorld()) return;

	//TODO: Cache the camera
	ALiquidCamera* liquidCamera = Cast<ALiquidPlayer>(GetOwner())->CameraHandler->LiquidCamera;
	check(liquidCamera)

	FuzzySelectStruct fuzzySelectStruct = FuzzySelectStruct();
	fuzzySelectStruct.SelectionType = FuzzySelectStruct::Class;
	fuzzySelectStruct.isInterface = false;
	fuzzySelectStruct.SubClass = ABaseEnemy::StaticClass();
	
	const FHitResult* enemyHit = liquidCamera->GetFuzzySelection(fuzzySelectStruct);
	
	if (enemyHit != nullptr)
	{
		if(enemyHit->GetActor())
		{
			TargettingActor = enemyHit->GetActor();
			EnableWidget(enemyHit->GetActor()->GetActorLocation());
			return;
		}
	}
	

	if (GetIsLockingOn())
	{
		if (IsValid(TargettingActor) && FVector::Dist(LiquidPlayer->GetActorLocation(),TargettingActor->GetActorLocation()) < ShootDistance)
		{
			EnableWidget(TargettingActor->GetActorLocation());
		}
		else
		{
			SetIsLockingOn(false);
		}
		return;
	}
	TargettingActor = nullptr;
	DisableWidget();
}

void UCombatComponent::EvaluateCombat()
{
	//Set the right combat mode
	if(PickedUpActor)
	{
		Throw();
	}
	else if(IsEnemyInCloseRange())
	{
		StartKick();
	}
	else
	{
		StartShooting();
	}
}

void UCombatComponent::EndEvaluateCombat()
{
	StopShooting();
	StopKick();
}

void UCombatComponent::Pickup(AThrowableActor* Actor)
{
	PickedUpActor = Actor;
}


bool UCombatComponent::HasPickedUpActor() const
{
	return PickedUpActor != nullptr;
}

void UCombatComponent::LockOn()
{
	if (bIsTargetting)
	{
		SetIsLockingOn(!GetIsLockingOn());
		if (GetIsLockingOn())
		{
			LiquidPlayer->FeedbackComponent->OnLockOn.Broadcast();
			LockedOnActor = TargettingActor;
		}
	}
	else
	{
		SetIsLockingOn(false);
	}
}

void UCombatComponent::StartShooting()
{
	if(	GetWorld()->GetTimerManager().IsTimerActive(ShootTimerHandle)) return;
	GetWorld()->GetTimerManager().SetTimer(ShootTimerHandle, this, &UCombatComponent::Shoot, ShootInterval, true, 0.0f);
}

void UCombatComponent::StopShooting()
{
	GetWorld()->GetTimerManager().ClearTimer(ShootTimerHandle);
}

void UCombatComponent::Shoot()
{

	if(IsEnemyInCloseRange())
	{
		StopShooting();
		StartKick();
	}

	
	const float energyCost = EnergyCostTable->FindRow<FEnergyConsumptionRow>(FName("Shoot"),FString())->EnergyConsumption;

	if (!LiquidPlayer->EnergyComponent->HasEnergy(energyCost))
	{
		return;
	}

	LiquidPlayer->EnergyComponent->LoseEnergy(energyCost);
	
	FVector rayTarget = GetTarget();
	
	// Spawn Niagara FX
	if (ShootingFX)
	{
		// Spawn the Niagara system at the same location as the bullet
		UNiagaraComponent* NiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			ShootingFX,
			GetOwner()->GetActorLocation(),
			GetOwner()->GetActorRotation() 
		);

		if (NiagaraComponent)
		{
			NiagaraComponent->SetVectorParameter(TEXT("EndPos"), rayTarget);
			NiagaraComponent->SetVectorParameter(TEXT("StartPos"), LiquidPlayer->SkeletalMeshComponent->GetSocketLocation("ShootingSocket"));
		}
	}
	LiquidPlayer->FeedbackComponent->OnShoot.Broadcast(LiquidPlayer->SkeletalMeshComponent->GetSocketLocation("ShootingSocket"), rayTarget);
	
	FHitResult hit;
	FCollisionQueryParams params; 
	params.AddIgnoredActor(GetOwner());
	float baseMultiplier = 1.f;
	baseMultiplier += LiquidPlayer->StuntComponent->GetCurrentMultiplier() * 0.15f;
	
	if(GetWorld()->LineTraceSingleByChannel(hit, GetOwner()->GetActorLocation(), rayTarget, ECollisionChannel::ECC_Camera, params))
	{
		if(hit.GetActor()->IsA<ABaseEnemy>())
		{
			ABaseEnemy* enemy = Cast<ABaseEnemy>(hit.GetActor());
			if (LiquidPlayer->HealthComponent->GetHealthPercentage() < 0.3f)
			{
				//Last Stand
				enemy->HealthComponent->TakeDamage(ShootDamage * 2 * baseMultiplier, UGameplayStatics::GetPlayerPawn(GetWorld(),0), GetOwner()->GetActorLocation());
			}
			else
			{
				enemy->HealthComponent->TakeDamage(ShootDamage * baseMultiplier, UGameplayStatics::GetPlayerPawn(GetWorld(),0), GetOwner()->GetActorLocation());
			}
		}
	}
}

bool UCombatComponent::IsEnemyInCloseRange() const
{
	if(bIsTargetting)
	{
		const float dist = FMath::Abs(FVector::Dist(TargettingLocation, GetOwner()->GetActorLocation()));
		if(dist < KickDistance)
		{
			return true;
		}
	}


	return false;
}

void UCombatComponent::StartKick()
{
	if(	GetWorld()->GetTimerManager().IsTimerActive(KickTimerHandle)) return;
	GetWorld()->GetTimerManager().SetTimer(KickTimerHandle, this, &UCombatComponent::Kick, KickInterval, true, 0.0f);
}

void UCombatComponent::StopKick()
{
	GetWorld()->GetTimerManager().ClearTimer(KickTimerHandle);
}

void UCombatComponent::Kick()
{
	if(!IsEnemyInCloseRange())
	{
		StopKick();
		StartShooting();
	}
	
	if(!TargettingActor) return;
	if(TargettingActor->IsA<ABaseEnemy>())
	{
		ABaseEnemy* enemy = Cast<ABaseEnemy>(TargettingActor);
		enemy->HealthComponent->TakeDamage(KickDamage, UGameplayStatics::GetPlayerPawn(GetWorld(),0), GetOwner()->GetActorLocation());
		enemy->EnableKick(GetOwner()->GetActorForwardVector() * KickForce);
		LiquidPlayer->FeedbackComponent->OnKick.Broadcast();
	}
	
}

void UCombatComponent::Throw()
{
	if(PickedUpActor == nullptr) return;
	FVector target = GetTarget();

	//Detach the actor
	//PickedUpActor->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	
	//Add force to the actor
	PickedUpActor->SetThrown((target - PickedUpActor->GetActorLocation()).GetSafeNormal() * ThrowSpeed);
	PickedUpActor->SetIsPickedUp(false);
	LiquidPlayer->FeedbackComponent->OnCombatObjectPushed.Broadcast();
	PickedUpActor = nullptr;
}

FVector UCombatComponent::GetTarget() const
{
	ALiquidCamera* liquidCamera = Cast<ALiquidPlayer>(GetOwner())->CameraHandler->LiquidCamera;
	FVector cameraLocation = liquidCamera->GetActorLocation();
	FVector cameraForward = liquidCamera->GetActorForwardVector();
	
	FVector rayTarget;
	if (bIsLockingOn)
	{
		rayTarget = TargettingLocation;
	}
	else
	{
		rayTarget = bIsTargetting ? TargettingLocation : cameraLocation + cameraForward * ShootDistance;
	}

	rayTarget = (rayTarget - GetOwner()->GetActorLocation()).GetSafeNormal() * ShootDistance + GetOwner()->GetActorLocation();
	return rayTarget;
}


void UCombatComponent::EnableWidget(const FVector& impactPoint)
{
	if (bIsLockingOn)
	{
		bIsTargetting = true;
		if (IsValid(TargettingActor))
		{
			TargettingLocation = TargettingActor->GetActorLocation();
			SetVisibility(true);
			SetWorldLocation(TargettingActor->GetActorLocation());;
		}
	}
	else
	{
		bIsTargetting = true;
		TargettingLocation = impactPoint;
		SetVisibility(true);
		SetWorldLocation(impactPoint);
	}

}

void UCombatComponent::DisableWidget()
{
	if (bIsLockingOn)
	{
		return;
	}
	bIsTargetting = false;
	SetVisibility(false);
}