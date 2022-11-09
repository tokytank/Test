// Copyright Epic Games, Inc. All Rights Reserved.

#include "TestGameMode.h"
#include "TestPlayerController.h"
#include "TestCharacter.h"
#include "GameFramework/GameMode.h"
#include "UObject/ConstructorHelpers.h"

ATestGameMode::ATestGameMode()
{
	// use our custom PlayerController class
	PlayerControllerClass = ATestPlayerController::StaticClass();

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/TopDown/Blueprints/BP_TopDownCharacter"));
	if (PlayerPawnBPClass.Class != nullptr)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
	// set default controller to our Blueprinted controller
	static ConstructorHelpers::FClassFinder<APlayerController> PlayerControllerBPClass(TEXT("/Game/TopDown/Blueprints/BP_TopDownPlayerController"));
	if(PlayerControllerBPClass.Class != NULL)
	{
		PlayerControllerClass = PlayerControllerBPClass.Class;
	}
}