#include "CameraHandler.h"

#include "Camera/CameraActor.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "LiquidGravity_Dev/Core/LiquidCamera.h"

UCameraHandler::UCameraHandler()
{
	PrimaryComponentTick.bCanEverTick = true;
}


void UCameraHandler::BeginPlay()
{
	Super::BeginPlay();

	if(TVCamera)
	{
		// Set the player controller's view target TV Camera
		UGameplayStatics::GetPlayerController(GetWorld(), 0)->SetViewTarget(TVCamera.Get());
		StartingCameraLocation = TVCamera->GetActorLocation();
		StartingCameraRotation = TVCamera->GetActorRotation();
	}
	
	SpawnCamera();

	//Lerp the camera from the TVCamera to the LiquidCamera
	OnTimelineUpdate.BindDynamic(this,&UCameraHandler::TransitionToLiquidCamera);
	CameraTimeline.AddInterpFloat(CameraCurve, OnTimelineUpdate);
	

}


void UCameraHandler::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	CameraTimeline.TickTimeline(DeltaTime);
}

void UCameraHandler::TransitionToMainCamera()
{
	CameraTimeline.PlayFromStart();
}

void UCameraHandler::TransitionToLiquidCamera(float alpha)
{
	if(LiquidCamera && TVCamera)
	{
		auto newLocation = FMath::Lerp(StartingCameraLocation, LiquidCamera->SpringArmComponent->GetSocketLocation(USpringArmComponent::SocketName),  alpha);
		auto newRotation = FMath::Lerp(StartingCameraRotation, LiquidCamera->GetActorRotation(), alpha);
		TVCamera->SetActorLocationAndRotation(newLocation, newRotation);
	}

	//use own alpha value instead of OnEndReached delegate for more control
	if(alpha >= 0.988f)
	{
		SetPlayerCameraActive();
	}
}

void UCameraHandler::SetPlayerCameraActive()
{
	if(LiquidCamera)
	{
		// Set the player controller's view target to the spawned camera	
		UGameplayStatics::GetPlayerController(GetWorld(), 0)->SetViewTarget(LiquidCamera);
		OnLiquidCameraActived.Broadcast();
	}
	
	CameraTimeline.Stop();
}

void UCameraHandler::SpawnCamera()
{
	ensure(CameraClass.Get());
	
	FActorSpawnParameters SpawnParams{};
	SpawnParams.Owner = GetOwner();
	SpawnParams.Instigator = GetOwner()->GetInstigator();
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	
	// Spawn the camera actor
	LiquidCamera = GetWorld()->SpawnActor<ALiquidCamera>(CameraClass, GetOwner()->GetActorLocation(), GetOwner()->GetActorRotation(), SpawnParams);
}

