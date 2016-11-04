// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "Task_3.h"
#include "Task_3GameMode.h"
#include "Task_3HUD.h"
#include "Task_3Character.h"

ATask_3GameMode::ATask_3GameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = ATask_3HUD::StaticClass();
}
