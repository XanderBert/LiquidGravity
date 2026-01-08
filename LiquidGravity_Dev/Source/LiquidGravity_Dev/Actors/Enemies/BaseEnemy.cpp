#include "BaseEnemy.h"

#include "NiagaraFunctionLibrary.h"
#include "AI/EnemyAiController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "Components/AudioComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "LiquidGravity_Dev/Actors/ThrowableActor.h"
#include "LiquidGravity_Dev/Components/EnemyShootingComponent.h"
#include "LiquidGravity_Dev/Components/HealthComponent.h"
#include "LiquidGravity_Dev/Components/WaypointUsageComponent.h"
#include "LiquidGravity_Dev/Components/PlayerComponents/PlayerFeedbackComponent.h"
#include "LiquidGravity_Dev/Components/PlayerComponents/PlayerStuntComponent.h"
#include "LiquidGravity_Dev/Core/LiquidGameInstance.h"
#include "LiquidGravity_Dev/Core/LiquidPlayer.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"


ABaseEnemy::ABaseEnemy()
{
	PrimaryActorTick.bCanEverTick = true;

	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule Component"));
	SetRootComponent(CapsuleComponent);
	CapsuleComponent->SetEnableGravity(false);
	CapsuleComponent->SetSimulatePhysics(false);
	SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Skeletal Mesh Component"));
	SkeletalMeshComponent->SetupAttachment(CapsuleComponent);

	EnemyShootingComponent = CreateDefaultSubobject<UEnemyShootingComponent>(TEXT("Enemy Shooting Component"));

	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("Health Component"));
	HealthComponent->OnPlayerDamaged.AddDynamic(this, &ABaseEnemy::NotifyAISystemOnShoot);
	EnemySound = CreateDefaultSubobject<UAudioComponent>(TEXT("EnemySound"));

	WaypointUsageComponent = CreateDefaultSubobject<UWaypointUsageComponent>(TEXT("Waypoint Usage Component"));
	
	AIPerceptionStimuliSourceComponent = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("AI Perception Stimuli Source Component"));


	OnPlayerSpotted.AddDynamic(this, &ABaseEnemy::NotifyEnemiesInRange);
	OnPlayerSpotted.AddDynamic(this, &ABaseEnemy::SetSpotsPlayer);
	OnPlayerLost.AddDynamic(this, &ABaseEnemy::SetLostPlayer);
	

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

float ABaseEnemy::GetKickMultiplier()
{
	return KickForceMultiplier;
}

void ABaseEnemy::BeginPlay()
{
	Super::BeginPlay();

	if(IdleSounds.Num() > 0)
	{
		int randomIndex = FMath::RandRange(0, IdleSounds.Num() - 1);
		EnemySound->SetSound(IdleSounds[randomIndex]);
	}
	HealthComponent->OnPlayerDied.AddDynamic(this, &ABaseEnemy::DestroyEnemy);
	EnemySound->Play();
}

void ABaseEnemy::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	RandomAnimationTimer+=DeltaSeconds;
	EnemySound->SetVolumeMultiplier(0.5f * Cast<ULiquidGameInstance>(GetGameInstance())->SoundVolume);
}

void ABaseEnemy::EnableKick(const FVector& impulse)
{
	if(IsBeingKick) return;
	
	//Get the AI Controller
	AEnemyAiController* controller = Cast<AEnemyAiController>(GetController());
	if(!IsValid(controller)) return;

	//Enable Kick
	IsBeingKick = true;
	
	//TODO: Restart Behaviour Tree in a elegant way so we directly go into kicking state
	controller->BehaviorComp->RestartTree();
	
	//Apply the impulse to the enemy
	CapsuleComponent->SetSimulatePhysics(true);
	CapsuleComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	SkeletalMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void ABaseEnemy::DisableKick()
{
	IsBeingKick = false;
	CapsuleComponent->SetSimulatePhysics(false);
	SkeletalMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CapsuleComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	
	//Remove the force that was applied to the enemy from the impulse.
	//This shouldn't be needed but just in case.
	CapsuleComponent->SetPhysicsLinearVelocity(FVector::ZeroVector);
}

bool ABaseEnemy::GetIsKick() const
{
	return IsBeingKick;
}

void ABaseEnemy::NotifyEnemiesInRange()
{
	//Get the player
	auto controller = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	APawn* pawn = controller->GetPawn();
	if(!IsValid(pawn)) return;

	
	
	// Use a sphere overlap query instead of getting all actors
	TArray<FOverlapResult> OverlapResults;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	GetWorld()->OverlapMultiByChannel(
		OverlapResults,
		pawn->GetActorLocation(),
		FQuat::Identity,
		ECC_Pawn,
		FCollisionShape::MakeSphere(NotifyOtherEnemiesRange),
		QueryParams
	);


	//Debug the sphere
	//DrawDebugSphere(GetWorld(), pawn->GetActorLocation(), NotifyOtherEnemiesRange, 12, FColor::Red, false, 10.f);



	
	for (const FOverlapResult& Result : OverlapResults)
	{
		if (ABaseEnemy* Enemy = Cast<ABaseEnemy>(Result.GetActor()))
		{
			Enemy->NotifyAISystem();
		}
	}
}

void ABaseEnemy::PlaySound(USoundBase* Sound, float Volume, float minPitch, float maxPitch) const
{
	if (IsValid(GetGameInstance()))
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), Sound, GetActorLocation(), Volume *
		Cast<ULiquidGameInstance>(GetGameInstance())->SoundVolume,
		FMath::RandRange(minPitch, maxPitch),0.f,Cast<ULiquidGameInstance>(GetGameInstance())->SoundAttenuation);
}

void ABaseEnemy::NotifyAISystem()
{
	if(!IsValid(this)) return;
	
	AEnemyAiController* controller = Cast<AEnemyAiController>(GetController());
	if(!IsValid(controller)) return;

	APawn* pawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if(!IsValid(pawn)) return;

	
	ALiquidPlayer* player = Cast<ALiquidPlayer>(pawn);
	if(!IsValid(player)) return;
	
	TookDamage = true;

	//Update the perception system
	controller->OnPerceptionUpdated({player});
}

void ABaseEnemy::NotifyAISystemOnShoot(int amount, AActor* damagingActor, FVector damageLocation)
{
	if(!IsValid(this)) return;
	
	if (HitSounds.Num() > 0)
	{
		int randomIndex = FMath::RandRange(0, HitSounds.Num() - 1);
		PlaySound(HitSounds[randomIndex],0.5f,0.7f,1.3f);
	}

	NotifyAISystem();
}

void ABaseEnemy::DestroyEnemy()
{
	ALiquidPlayer* player = Cast<ALiquidPlayer>(UGameplayStatics::GetPlayerPawn(GetWorld(),0));
	if(IsValid(player))
	{
		player->FeedbackComponent->OnEnemyKilled.Broadcast();
	}
	ULiquidGameInstance* gameInstance = Cast<ULiquidGameInstance>(GetGameInstance());
	if(IsValid(gameInstance))
	{
		gameInstance->EnemiesKilled++;
	}
	if (DeathSounds.Num() > 0)
	{
		int randomIndex = FMath::RandRange(0, DeathSounds.Num() - 1);
		PlaySound(DeathSounds[randomIndex],0.8f,0.7f,1.3f);
	}	
	AActor* ata = nullptr;
	if(IsValid(DeadBody) && bSpawnDeadBody)
	{
		FActorSpawnParameters params;
		ata = GetWorld()->SpawnActor<AActor>(DeadBody, SkeletalMeshComponent->GetComponentLocation(), SkeletalMeshComponent->GetComponentRotation());
	    AThrowableActor* ata2 = Cast<AThrowableActor>(ata);
		if (IsValid(ata2))
			ata2->CapsuleComponent->AddForce(
				(UGameplayStatics::GetPlayerPawn(GetWorld(),0)->GetActorLocation() - GetActorLocation()).GetSafeNormal() * 25000.f +
				UKismetMathLibrary::RandomUnitVector() * 5000.f,NAME_None,true);
	}

	if (DeathFX)
	{
		if (ata == nullptr)
		{
			// Spawn the Niagara system at the same location as the bullet
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(
				GetWorld(),
				DeathFX,
				SkeletalMeshComponent->GetComponentLocation(),
				SkeletalMeshComponent->GetComponentRotation()
			);
		}
	}
	FStuntStruct stuntStruct = FStuntStruct();
	stuntStruct.StuntName = EnemyName + " Killed";
	stuntStruct.ScoreAddition = ScoreOnAddition;
	ALiquidPlayer* alp = Cast<ALiquidPlayer>(UGameplayStatics::GetPlayerPawn(GetWorld(),0));
	if (IsValid(alp))
	{
		alp->StuntComponent->AddStuntStruct(stuntStruct);
	}
	Destroy();
}
