// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/GA_InventoryAbility.h"

#include "Actors/WeaponItemActor.h"
#include "Inventory/InventoryComponent.h"
#include "Inventory/InventoryItemInstance.h"

void UGA_InventoryAbility::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);

	InventoryComponent = ActorInfo->OwnerActor.Get()->FindComponentByClass<UInventoryComponent>();
}

UInventoryComponent* UGA_InventoryAbility::GetInventoryComponent()
{
	return InventoryComponent;
}

UInventoryItemInstance* UGA_InventoryAbility::GetEquippedItemInstance()
{
	return InventoryComponent ? InventoryComponent->GetEquippedItem() : nullptr;
}

const UItemStaticData* UGA_InventoryAbility::GetEquippedItemStaticData()
{
	UInventoryItemInstance* EquippedItem = GetEquippedItemInstance();
	return EquippedItem ? EquippedItem->GetItemStaticData() : nullptr;
	
}

const UWeaponStaticData* UGA_InventoryAbility::GetEquippedItemWeaponStaticData()
{
	return Cast<UWeaponStaticData>(GetEquippedItemStaticData());
}

AItemActor* UGA_InventoryAbility::GetEquippedItemActor()
{
	UInventoryItemInstance* EquippedItem = GetEquippedItemInstance();
	return EquippedItem ? EquippedItem->GetItemActor() : nullptr;
}

AWeaponItemActor* UGA_InventoryAbility::GetEquippedWeaponItemActor()
{
	return Cast<AWeaponItemActor>(GetEquippedItemActor());
}
