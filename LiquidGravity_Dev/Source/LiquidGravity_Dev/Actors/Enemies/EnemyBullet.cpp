#include "EnemyBullet.h"

#include "BaseEnemy.h"
#include "Components/SphereComponent.h"
#include "LiquidGravity_Dev/Components/HealthComponent.h"
#include "LiquidGravity_Dev/Core/LiquidPlayer.h"


AEnemyBullet::AEnemyBullet()
{
	PrimaryActorTick.bCanEverTick = true;
	BulletCollision = CreateDefaultSubobject<USphereComponent>(TEXT("Bullet Collision"));
	SetRootComponent(BulletCollision);
	BulletCollision->SetGenerateOverlapEvents(true);

	//No Visibility and Camera overlap
	BulletCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	BulletCollision->SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel1);
	BulletCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	BulletCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Ignore);
	BulletCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

	bIsShot = true;

	BulletCollision->OnComponentBeginOverlap.AddDynamic(this, &AEnemyBullet::OnBulletOverlap);


	BulletMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Bullet Mesh"));
	BulletMesh->SetupAttachment(BulletCollision);

	//Ignore all collisions
	BulletMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AEnemyBullet::BeginPlay()
{
	Super::BeginPlay();
	SpawnLocation = GetActorLocation();
}


void AEnemyBullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(bIsShot)
	{
		// Move the bullet
		FVector NewLocation = GetActorLocation() + Direction * Speed * DeltaTime;
		SetActorLocation(NewLocation, true);
	}
}

void AEnemyBullet::Shoot(const FVector& direction, float speed)
{
	GetWorldTimerManager().SetTimer(DestroyTimer, this, &AEnemyBullet::DestroyBullet, LifeTime, false);
	
	Direction = direction.GetSafeNormal();
	Speed = speed;
}

void AEnemyBullet::DestroyBullet()
{
	Destroy();
}

void AEnemyBullet::OnBulletOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	auto name = OtherActor->GetName();
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Bullet Hit: %s"), *name));
	
	if(OtherActor->IsA<AEnemyBullet>() || OtherActor->IsA<ABaseEnemy>())
	{
		return;
	}
	
	if(OtherActor->IsA<ALiquidPlayer>() && bIsShot)
	{
		//TODO:  Deal damage to the player
		ALiquidPlayer* player = Cast<ALiquidPlayer>(OtherActor);
		if (player->HealthComponent->GetHealthPercentage() < 0.3f)
		{
			//Last Stand
			player->HealthComponent->TakeDamage(Damage/2.f, BulletShootingActor, GetActorLocation());
		}
		else
		{
			player->HealthComponent->TakeDamage(Damage, BulletShootingActor, GetActorLocation());
		}
		
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Player Hit"));
	}

	GetWorldTimerManager().ClearTimer(DestroyTimer);
	Destroy();
}