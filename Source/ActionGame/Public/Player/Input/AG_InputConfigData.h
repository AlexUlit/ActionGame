﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "InputAction.h"
#include "AG_InputConfigData.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONGAME_API UAG_InputConfigData : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UInputAction* InputMove;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UInputAction* InputLook;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UInputAction* Jump;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UInputAction* Crouch;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UInputAction* Sprint;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UInputAction* DropItem;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UInputAction* EquipNextItem;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UInputAction* UnequipItem;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UInputAction* Attack;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UInputAction* Aim;
};
