// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/AG_PlayerController.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "ActionGame/ActionGameGameModeBase.h"

void AAG_PlayerController::RestartPlayerIn(float InTime)
{
	ChangeState(NAME_Spectating);

	GetWorld()->GetTimerManager().SetTimer(RestartPlayerTimerHandle, this, &AAG_PlayerController::RestartPlayer, InTime, false);
}

void AAG_PlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	if (UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(InPawn))
	{
		DeathStateTagDelegate = ASC->RegisterGameplayTagEvent(FGameplayTag::RequestGameplayTag(TEXT("State.Dead")), EGameplayTagEventType::NewOrRemoved).AddUObject(this, &AAG_PlayerController::OnPawnDeathStateChanged);
	}
}

void AAG_PlayerController::OnUnPossess()
{
	Super::OnUnPossess();
	if (DeathStateTagDelegate.IsValid())
	{
		if (UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn()))
		{
			ASC->UnregisterGameplayTagEvent(DeathStateTagDelegate, FGameplayTag::RequestGameplayTag(TEXT("State.Dead")), EGameplayTagEventType::NewOrRemoved);
		}
	}
}

void AAG_PlayerController::OnPawnDeathStateChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	if (NewCount > 0)
	{
		UWorld* World= GetWorld();
		AActionGameGameModeBase* GameMode = World ? Cast<AActionGameGameModeBase>(World->GetAuthGameMode()) : nullptr;
		
		if (GameMode)
		{
			GameMode->NotifyPlayerDied(this);
		}

		if (UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn()))
		{
			ASC->UnregisterGameplayTagEvent(DeathStateTagDelegate, FGameplayTag::RequestGameplayTag(TEXT("State.Dead")), EGameplayTagEventType::NewOrRemoved);
		}
	}
}

void AAG_PlayerController::RestartPlayer()
{
	UWorld* World= GetWorld();
	AActionGameGameModeBase* GameMode = World ? Cast<AActionGameGameModeBase>(World->GetAuthGameMode()) : nullptr;

	if (GameMode)
	{
		GameMode->RestartPlayer(this);	
	}
}
