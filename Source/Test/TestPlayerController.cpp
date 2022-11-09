// Copyright Epic Games, Inc. All Rights Reserved.

#include "TestPlayerController.h"
#include "GameFramework/Pawn.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "TestCharacter.h"
#include "Engine/World.h"
#include"Kismet/GameplayStatics.h"
#include "GameFramework/SpringArmComponent.h"
#include"Test/Public/Damageable.h"

ATestPlayerController::ATestPlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;
}

void ATestPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	if(bInputPressed)
	{
		FollowTime += DeltaTime;

		// Look for the touch location
		FVector HitLocation = FVector::ZeroVector;
		FHitResult Hit;
		if(bIsTouch)
		{
			GetHitResultUnderFinger(ETouchIndex::Touch1, ECC_Visibility, true, Hit);
		}
		else
		{
			GetHitResultUnderCursor(ECC_Visibility, true, Hit);
		}
		HitLocation = Hit.Location;

		// Direct the Pawn towards that location
		APawn* const MyPawn = GetPawn();
		if(MyPawn)
		{
			FVector WorldDirection = (HitLocation - MyPawn->GetActorLocation()).GetSafeNormal();
			MyPawn->AddMovementInput(WorldDirection, 1.f, false);
		}
	}
	else
	{
		FollowTime = 0.f;
	}
}

void ATestPlayerController::SetupInputComponent()
{
	// set up gameplay key bindings
	Super::SetupInputComponent();

	InputComponent->BindAction("SetDestination", IE_Pressed, this, &ATestPlayerController::OnSetDestinationPressed);
	InputComponent->BindAction("SetDestination", IE_Released, this, &ATestPlayerController::OnSetDestinationReleased);

	InputComponent->BindAction("UpView", IE_Pressed, this, &ATestPlayerController::UpView);
	InputComponent->BindAction("DownView", IE_Pressed, this, &ATestPlayerController::DownView);
	InputComponent->BindAction("ResetView", IE_Pressed, this, &ATestPlayerController::ResetView);
	// support touch devices 
	InputComponent->BindTouch(EInputEvent::IE_Pressed, this, &ATestPlayerController::OnTouchPressed);
	InputComponent->BindTouch(EInputEvent::IE_Released, this, &ATestPlayerController::OnTouchReleased);

}

void ATestPlayerController::OnSetDestinationPressed()
{
	// We flag that the input is being pressed
	bInputPressed = true;
	// Just in case the character was moving because of a previous short press we stop it
	StopMovement();
}

void ATestPlayerController::OnSetDestinationReleased()
{
	// Player is no longer pressing the input
	bInputPressed = false;

	// If it was a short press
	if(FollowTime <= ShortPressThreshold)
	{
		// We look for the location in the world where the player has pressed the input
		FVector HitLocation = FVector::ZeroVector;
		FHitResult Hit;
		GetHitResultUnderCursor(ECC_Visibility, true, Hit);
		HitLocation = Hit.Location;

		// We move there and spawn some particles
		UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, HitLocation);
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, FXCursor, HitLocation, FRotator::ZeroRotator, FVector(1.f, 1.f, 1.f), true, true, ENCPoolMethod::None, true);
	}
}

void ATestPlayerController::OnTouchPressed(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	bIsTouch = true;
	OnSetDestinationPressed();
}

void ATestPlayerController::OnTouchReleased(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	bIsTouch = false;
	OnSetDestinationReleased();
}

void ATestPlayerController::UpView()
{
	
	TObjectPtr<ATestCharacter>player=Cast<ATestCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	auto arm=player->GetCameraBoom();
	if (arm->TargetArmLength > 500.f)
		arm->TargetArmLength -= 50.f;
}

void ATestPlayerController::DownView()
{
	TObjectPtr<ATestCharacter>player = Cast<ATestCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	
	if (player!=nullptr)
	{
		Cast<IDamageable>(player)->Damage(30);
		auto arm = player->GetCameraBoom();
		if (arm->TargetArmLength < 3000.f)
			arm->TargetArmLength += 50.f;
	}
	
}

void ATestPlayerController::ResetView()
{
	TObjectPtr<ATestCharacter>player = Cast<ATestCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	if (player != nullptr)
	{
		auto arm = player->GetCameraBoom();
		arm->TargetArmLength = 1400.f;

	}
}
