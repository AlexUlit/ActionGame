// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AG_GameplayAbility.h"
#include "GA_InventoryAbility.generated.h"

class UInventoryComponent;
class UInventoryItemInstance;
class UWeaponStaticData;
class AItemActor;
class AWeaponItemActor;

UCLASS()
class ACTIONGAME_API UGA_InventoryAbility : public UAG_GameplayAbility
{
	GENERATED_BODY()

public:
	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;

	UFUNCTION(BlueprintPure)
	UInventoryComponent* GetInventoryComponent();

	UFUNCTION(BlueprintPure)
	UInventoryItemInstance* GetEquippedItemInstance();

	UFUNCTION(BlueprintPure)
	const UItemStaticData* GetEquippedItemStaticData();

	UFUNCTION(BlueprintPure)
	const UWeaponStaticData* GetEquippedItemWeaponStaticData();

	UFUNCTION(BlueprintPure)
	AItemActor* GetEquippedItemActor();

	UFUNCTION(BlueprintPure)
	AWeaponItemActor* GetEquippedWeaponItemActor();

protected:
	UPROPERTY()
	UInventoryComponent* InventoryComponent = nullptr;
};
