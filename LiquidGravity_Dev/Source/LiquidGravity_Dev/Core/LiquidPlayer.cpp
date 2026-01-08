#include "LiquidPlayer.h"

#include "LiquidController.h"
#include "LiquidGameInstance.h"
#include "LiquidHUD.h"
#include "Components/AudioComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StateTreeComponent.h"

#include "Kismet/GameplayStatics.h"
#include "LiquidGravity_Dev/Actors/Enemies/BaseEnemy.h"
#include "LiquidGravity_Dev/Actors/Enemies/Ringo.h"
#include "LiquidGravity_Dev/Components/PlayerComponents/ChangeGravityComponent.h"
#include "LiquidGravity_Dev/Components/EnergyComponent.h"
#include "LiquidGravity_Dev/Components/HealthComponent.h"
#include "LiquidGravity_Dev/Components/PlayerComponents/CameraHandler.h"
#include "LiquidGravity_Dev/Components/PlayerComponents/InteractComponent.h"
#include "LiquidGravity_Dev/Components/PlayerComponents/LiquidMovementComponent.h"
#include "LiquidGravity_Dev/Components/PlayerComponents/PlayerHookingComponent.h"
#include "LiquidGravity_Dev/Components/PlayerComponents/CombatComponent.h"
#include "LiquidGravity_Dev/Components/PlayerComponents/PickupComponent.h"
#include "LiquidGravity_Dev/Components/PlayerComponents/PlayerFeedbackComponent.h"
#include "LiquidGravity_Dev/Components/PlayerComponents/PlayerStuntComponent.h"

ALiquidPlayer::ALiquidPlayer()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bTickEvenWhenPaused = true;

	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule Component"));
	SetRootComponent(CapsuleComponent);
	CapsuleComponent->SetEnableGravity(false);
	CapsuleComponent->SetSimulatePhysics(true);
	CapsuleComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	
	
	SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Skeletal Mesh Component"));
	SkeletalMeshComponent->SetupAttachment(CapsuleComponent);

	DefaultOverlappingSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Default Overlapping Sphere"));
	DefaultOverlappingSphere->SetupAttachment(CapsuleComponent);

	
	FootOverlappingSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Foot Overlapping Sphere"));
	FootOverlappingSphere->SetupAttachment(CapsuleComponent);

	PickupComponent = CreateDefaultSubobject<UPickupComponent>(TEXT("Pickup Component"));
	PickupComponent->SetupAttachment(CapsuleComponent);

	LiquidMovementComponent = CreateDefaultSubobject<ULiquidMovementComponent>(TEXT("Movement Component"));
	StateTreeComponent = CreateDefaultSubobject<UStateTreeComponent>(TEXT("State Tree Component"));
	ChangeGravityComponent = CreateDefaultSubobject<UChangeGravityComponent>(TEXT("Change Gravity Component"));
	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("Health Component"));
	EnergyComponent = CreateDefaultSubobject<UEnergyComponent>(TEXT("Energy Component"));
	CombatComponent = CreateDefaultSubobject<UCombatComponent>(TEXT("Shooting Component"));
	HookingComponent = CreateDefaultSubobject<UPlayerHookingComponent>(TEXT("Hooking Component"));
	InteractComponent = CreateDefaultSubobject<UInteractComponent>(TEXT("Interact Component"));
	StuntComponent = CreateDefaultSubobject<UPlayerStuntComponent>(TEXT("Stunt Component"));
	FeedbackComponent = CreateDefaultSubobject<UPlayerFeedbackComponent>(TEXT("Feedback Component"));
	CameraHandler = CreateDefaultSubobject<UCameraHandler>(TEXT("Camera Handler"));
	
	SkatingSound = CreateDefaultSubobject<UAudioComponent>(TEXT("SkatingSound"));
	SkatingSound->SetupAttachment(RootComponent);
	GrindingSound = CreateDefaultSubobject<UAudioComponent>(TEXT("GrindingSound"));
	GrindingSound->SetupAttachment(RootComponent);
	
	MusicComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("MusicComponent"));
	MusicComponent->SetupAttachment(RootComponent);
}

void ALiquidPlayer::OnPlayerDamaged(int Amount, AActor* DamagingActor, FVector DamageLocation)
{
	ULiquidGameInstance* gameInstance = Cast<ULiquidGameInstance>(GetGameInstance());
	if(IsValid(gameInstance))
	{
		gameInstance->DamageTaken += Amount;
	}
}

void ALiquidPlayer::BeginPlay()
{
	Super::BeginPlay();

	MusicComponent->Stop();
	
	//Get player controller
	ALiquidController* controller = Cast<ALiquidController>(GetController());
	controller->OnTrailerSkippedOrEnded.AddDynamic(this, &ALiquidPlayer::SetupMusicAndPlay);
	controller->OnTrailerSkippedOrEnded.AddDynamic(CameraHandler, &UCameraHandler::TransitionToMainCamera);
	
	HealthComponent->OnPlayerDamaged.AddDynamic(this, &ALiquidPlayer::OnPlayerDamaged);

	GetWorldTimerManager().SetTimer(CheckHighItensityTimer, this, &ALiquidPlayer::SetHighIntensityMusicWhenNeeded, 1.f, true);
}


void ALiquidPlayer::SetHighIntensityMusicWhenNeeded()
{
	bool shouldPlayHighIntensity = false;
	
	ULiquidGameInstance* gameInstance = Cast<ULiquidGameInstance>(GetGameInstance());
	auto ringos = gameInstance->GetRingoActors();

	for (auto* ringo : ringos)
	{
		auto currentState = Cast<ABaseEnemy>(ringo)->CurrentAnimationState;
		
		if( currentState == UAnimationState::Attacking || currentState == UAnimationState::HeavyAttack || currentState == UAnimationState::PlayerDiscovered)
		{
			shouldPlayHighIntensity = true;
			break;
		}
	}
	if(shouldPlayHighIntensity)
	{
		SetHighIntensityMusic();
	}else
	{
		SetLowIntensityMusic();
	}
}

void ALiquidPlayer::SetupMusicAndPlay()
{
	//Setup The music
	auto controller = Cast<ALiquidController>(GetController());
	auto liqduiHud = Cast<ALiquidHUD>(controller->GetHUD());
	if(liqduiHud)
	{
		liqduiHud->OnDisableMenu.AddDynamic(this, &ALiquidPlayer::SetMusicToGame);
		liqduiHud->OnPause.AddDynamic(this, &ALiquidPlayer::SetMusicToMenu);
		liqduiHud->OnUnpause.AddDynamic(this, &ALiquidPlayer::SetMusicToGame);
		
	}

	SetLowIntensityMusic();
	SetMusicToMenu();

	float instanceMultiplier = Cast<ULiquidGameInstance>(GetGameInstance())->MusicVolume;
	MusicComponent->SetVolumeMultiplier(instanceMultiplier);
	MusicComponent->Play();
	HealthComponent->OnPlayerDamaged.AddDynamic(this, &ALiquidPlayer::SetToMediumHighForPeriod);

	
	//Get all ringo's in the level
	TArray<AActor*> ringos;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ARingo::StaticClass(), ringos);
	for(auto ringo : ringos)
	{
		auto castedRingo = Cast<ARingo>(ringo);
		if(castedRingo)
		{
			castedRingo->HealthComponent->OnPlayerDied.AddDynamic(this, &ALiquidPlayer::SetLowIntensityMusic);
		}
	}
}

void ALiquidPlayer::SetMusicToMenu()
{
	MusicComponent->SetSound(MenuMusic);
	MusicComponent->Play();
}

void ALiquidPlayer::SetMusicToGame()
{
	MusicComponent->SetSound(GameMusic);
	MusicComponent->Play();
}

void ALiquidPlayer::SetHighIntensityMusic()
{
	MusicComponent->SetFloatParameter(FName("Intensity"), 2.f);
}

void ALiquidPlayer::SetLowIntensityMusic()
{
	//Get al ringos and check if they spotted the player
	TArray<AActor*> ringos;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ARingo::StaticClass(), ringos);
	for(auto ringo : ringos)
	{
		auto castedRingo = Cast<ARingo>(ringo);
		if(castedRingo)
		{
			if(castedRingo->SpotsPlayer())
			{
				SetHighIntensityMusic();
				return;
			}
		}
	}
	
	MusicComponent->SetFloatParameter(FName("Intensity"), 0.f);
}

void ALiquidPlayer::SetToMediumHighForPeriod(int Amount, AActor* DamagingActor, FVector DamageLocation)
{
	MusicComponent->SetFloatParameter(FName("Intensity"), 1.f);

	//Set to 0 after few secondsf
	FTimerHandle timerHandle;
	GetWorld()->GetTimerManager().SetTimer(timerHandle, this, &ALiquidPlayer::SetLowIntensityMusic, 30.f, false);
}
