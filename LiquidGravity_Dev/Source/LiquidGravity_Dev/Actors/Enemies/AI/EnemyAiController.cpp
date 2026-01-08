#include "EnemyAiController.h"

#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "LiquidGravity_Dev/Actors/Enemies/BaseEnemy.h"
#include "LiquidGravity_Dev/Core/LiquidPlayer.h"
#include "Perception/AIPerceptionComponent.h"

AEnemyAiController::AEnemyAiController()
{
	PrimaryActorTick.bCanEverTick = true;
	BlackboardComp = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackBoardComp"));
	BehaviorComp = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorComp"));
	
	AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerceptionComponent"));
	AIPerceptionComponent->OnPerceptionUpdated.AddDynamic(this, &AEnemyAiController::OnPerceptionUpdated);
	SetPerceptionComponent(*AIPerceptionComponent);

	
	BrainComponent = BehaviorComp;
	bWantsPlayerState = true;
}

void AEnemyAiController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	
	ABaseEnemy* Bot = Cast<ABaseEnemy>(InPawn);

	// start behavior
	if (Bot && Bot->BehaviorTree)
	{
		if (Bot->BehaviorTree->BlackboardAsset)
		{
			BlackboardComp->InitializeBlackboard(*Bot->BehaviorTree->BlackboardAsset);
		}

		PlayerKeyID = BlackboardComp->GetKeyID("Player");
		NextLocationID = BlackboardComp->GetKeyID("NextLocation");
		SelfActorID = BlackboardComp->GetKeyID("SelfActor");
		BlackboardComp->SetValueAsObject("SelfActor", Bot);


		BehaviorComp->StartTree(*(Bot->BehaviorTree));
	}
}

void AEnemyAiController::OnUnPossess()
{
	Super::OnUnPossess();
	BehaviorComp->StopTree();
}

void AEnemyAiController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	//Get the owner bot
	const ABaseEnemy* bot = Cast<ABaseEnemy>(GetPawn());
	if(bot == nullptr) return;
	
	auto* player = Cast<ALiquidPlayer>(GetWorld()->GetFirstPlayerController()->GetPawn());

	if (player == nullptr) return;
	
	if(FMath::Abs(FVector::Dist(bot->GetActorLocation(), player->GetActorLocation())) < TouchDistance)
	{
		OnPerceptionUpdated({player});
	}
}

void AEnemyAiController::OnPerceptionUpdated(const TArray<AActor*>& UpdatedActors)
{

	ALiquidPlayer* detectedPlayer{};
	UObject* currentPlayerValue = BlackboardComp->GetValueAsObject("Player");
	
	// Find the player in the updated actors
	for (AActor* Actor : UpdatedActors)
	{
		if (ALiquidPlayer* Player = Cast<ALiquidPlayer>(Actor))
		{
			detectedPlayer = Player;
			break;
		}
	}
	
	if (detectedPlayer)
	{
		// Player detected
		if (currentPlayerValue != detectedPlayer)
		{
			// New player detected, update blackboard
			BlackboardComp->SetValueAsObject("Player", detectedPlayer);
			PlayerDiscovered = true;
			
			// Broadcast that the player has been spotted
			if (ABaseEnemy* Bot = Cast<ABaseEnemy>(GetPawn()))
			{
				Bot->OnPlayerSpotted.Broadcast();
				//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Silver, FString::Printf(TEXT("Player Spotted By: %s"), *Bot->GetName()));
			}
		}
	}
}