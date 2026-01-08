#include "EnemyShootingComponent.h"

#include "Kismet/KismetMathLibrary.h"
#include "LiquidGravity_Dev/Actors/Enemies/BaseEnemy.h"
#include "LiquidGravity_Dev/Actors/Enemies/EnemyBullet.h"


UEnemyShootingComponent::UEnemyShootingComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UEnemyShootingComponent::Shoot(FName socket) const
{
	ShootBullet(BulletClass, socket);
}

void UEnemyShootingComponent::HeavyShoot(FName socket) const
{
	ShootBullet(HeavyAttackBulletClass, socket);
}

void UEnemyShootingComponent::ShootBullet(TSubclassOf<AEnemyBullet> bulletClassToSpawn, FName socket) const
{
	if (!bIsAttacking) return;
	if(bulletClassToSpawn.Get())
	{
		//Direction to player
		APawn* player = GetWorld()->GetFirstPlayerController()->GetPawn();
		FRotator rotation = UKismetMathLibrary::FindLookAtRotation(GetOwner()->GetActorLocation(), player->GetActorLocation());
		FVector shootingSocketLocation = Cast<ABaseEnemy>(GetOwner())->SkeletalMeshComponent->GetSocketLocation(socket);
		
		// Spawn bullet
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = GetOwner();
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		AEnemyBullet* bullet = GetWorld()->SpawnActor<AEnemyBullet>(BulletClass.Get(), shootingSocketLocation, rotation , SpawnParams);

		if(bullet)
		{
			FVector direction = (player->GetActorLocation() - shootingSocketLocation).GetSafeNormal();
			bullet->SetBulletShootingActor(GetOwner());
			bullet->Shoot(direction, BulletSpeed);
		}
	}
}

