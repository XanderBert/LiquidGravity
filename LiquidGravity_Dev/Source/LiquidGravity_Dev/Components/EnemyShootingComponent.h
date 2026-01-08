// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EnemyShootingComponent.generated.h"


class AEnemyBullet;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LIQUIDGRAVITY_DEV_API UEnemyShootingComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UEnemyShootingComponent();

	bool bIsAttacking{};
	void Shoot(FName socket) const;
	void HeavyShoot(FName socket) const;
	
private:
	UPROPERTY(EditAnywhere)
	TSubclassOf<AEnemyBullet> BulletClass;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AEnemyBullet> HeavyAttackBulletClass;

	
	UPROPERTY(EditAnywhere)
	float BulletSpeed = 1000.f;

	void ShootBullet(TSubclassOf<AEnemyBullet> bulletClassToSpawn, FName socket) const;

};
