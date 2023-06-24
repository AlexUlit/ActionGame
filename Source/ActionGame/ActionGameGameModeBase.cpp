// Copyright Epic Games, Inc. All Rights Reserved.


#include "ActionGameGameModeBase.h"

#include "Player/AG_PlayerController.h"


AActionGameGameModeBase::AActionGameGameModeBase()
{
	PlayerControllerClass = AAG_PlayerController::StaticClass();
}

void AActionGameGameModeBase::NotifyPlayerDied(AAG_PlayerController* PlayerController)
{
	if (PlayerController)
	{
		PlayerController->RestartPlayerIn(2.f);
	}
} 
