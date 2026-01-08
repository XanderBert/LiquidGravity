#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "Components/WidgetComponent.h"
#include "LiquidGravity_Dev/Actors/HookableActor.h"
#include "PlayerHookingComponent.generated.h"


class UWidget;
class UWidgetBlueprint;
class ALiquidPlayer;
class ALiquidCamera;


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnHookStartedSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnHookEndedSignature);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LIQUIDGRAVITY_DEV_API UPlayerHookingComponent : public UWidgetComponent
{
	GENERATED_BODY()

public:
	UPlayerHookingComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	void EnableWidget(const FVector& impactPoint);
	void DisableWidget();

	UPROPERTY(BlueprintAssignable)
	FOnHookStartedSignature OnHookStarted;

	UPROPERTY(BlueprintAssignable)
	FOnHookEndedSignature OnHookEnded;

	AHookableActor* GetTargetHook() const;

	void TryHook() const;
	void RemoveHookTag() const;
	
private:
	void RenderHookWidget() const;
	
	UPROPERTY()
	ALiquidPlayer* LiquidPlayer{};

	UPROPERTY()
	AHookableActor* HookActor{};
	
	FCollisionQueryParams CollisionQueryParams;


	UPROPERTY(EditAnywhere)
	float DistanceToHook{ 1000.f };

	UPROPERTY(EditAnywhere)
	float RadiusToCheckForHook{ 200.f };
	

	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> HookWidgetBlueprint{};

	UPROPERTY()
	UUserWidget* HookWidget{};

	UPROPERTY(EditAnywhere)
	FGameplayTag HookTag;
};
