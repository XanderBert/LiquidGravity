#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemyBullet.generated.h"

UCLASS()
class LIQUIDGRAVITY_DEV_API AEnemyBullet : public AActor
{
	GENERATED_BODY()

public:
	AEnemyBullet();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	//-------------------- Components --------------------
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	class USphereComponent* BulletCollision;

	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	class UStaticMeshComponent* BulletMesh;

	//-------------------- Movement --------------------
	void Shoot(const FVector& direction, float speed);

	void SetBulletShootingActor(AActor* actor) { BulletShootingActor = actor; }

private:
	FTimerHandle DestroyTimer;

	UPROPERTY(EditAnywhere, Category = "Bullet")
	float LifeTime = 5.0f;

	FVector SpawnLocation;

	
	UPROPERTY(EditAnywhere, Category = "Bullet")
	float Damage = 5.0f;
	
	void DestroyBullet();

	UFUNCTION()
	void OnBulletOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	FVector Direction;
	float Speed;
	bool bIsShot = false;

	UPROPERTY()
	AActor* BulletShootingActor;
};
