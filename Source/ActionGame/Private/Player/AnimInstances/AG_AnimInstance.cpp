// Fill out your copyright notice in the Description page of Project Settings.

#include "Player/AnimInstances/AG_AnimInstance.h"
#include "ActionGame/DataAssets/CharacterAnimDataAsset.h"
#include "Inventory/InventoryComponent.h"
#include "Inventory/InventoryItemInstance.h"
#include "Player/AG_Character.h"


const UItemStaticData* UAG_AnimInstance::GetEquippedItemData() const
{
		AAG_Character* ActionGameCharacter = Cast<AAG_Character>(GetOwningActor());
		UInventoryComponent* InventoryComponent = ActionGameCharacter ? ActionGameCharacter->GetInventoryComponent() : nullptr;
		UInventoryItemInstance* ItemInstance = InventoryComponent ? InventoryComponent->GetEquippedItem() : nullptr;
		return ItemInstance ? ItemInstance->GetItemStaticData() : nullptr;
	
}

UBlendSpace* UAG_AnimInstance::GetLocomotionBlendSpace() const
{
	if (AAG_Character* ActionGameCharacter = Cast<AAG_Character>(GetOwningActor()))
	{
		if (const UItemStaticData* ItemData = GetEquippedItemData())
		{
			if (ItemData->CharacterAnimationData.MovementBlendSpace)
			{
				return ItemData->CharacterAnimationData.MovementBlendSpace;
			}
		}
		FCharacterData Data = ActionGameCharacter->GetCharacterData();
		if (Data.CharacterAnimDataAsset)
		{
			return Data.CharacterAnimDataAsset->CharacterAnimationData.MovementBlendSpace;
		}
	}
	return DefaultCharacterAnimDataAsset ? DefaultCharacterAnimDataAsset->CharacterAnimationData.MovementBlendSpace : nullptr;
}

UAnimSequenceBase* UAG_AnimInstance::GetIdleAnimation() const
{
	if (const UItemStaticData* ItemData = GetEquippedItemData())
	{
		if (ItemData->CharacterAnimationData.IdleAnimationAsset)
		{
			return ItemData->CharacterAnimationData.IdleAnimationAsset;
		}
	}
	
	if (AAG_Character* ActionGameCharacter = Cast<AAG_Character>(GetOwningActor()))
	{
		FCharacterData Data = ActionGameCharacter->GetCharacterData();
		if (Data.CharacterAnimDataAsset)
		{
			return Data.CharacterAnimDataAsset->CharacterAnimationData.IdleAnimationAsset;
		}
	}
	return DefaultCharacterAnimDataAsset ? DefaultCharacterAnimDataAsset->CharacterAnimationData.IdleAnimationAsset : nullptr;
}

UBlendSpace* UAG_AnimInstance::GetCrouchBlendSpace() const
{
	if (const UItemStaticData* ItemData = GetEquippedItemData())
	{
		if (ItemData->CharacterAnimationData.CrouchBlendSpace)
		{
			return ItemData->CharacterAnimationData.CrouchBlendSpace;
		}
	}
	
	if (AAG_Character* ActionGameCharacter = Cast<AAG_Character>(GetOwningActor()))
	{
		FCharacterData Data = ActionGameCharacter->GetCharacterData();
		if (Data.CharacterAnimDataAsset)
		{
			return Data.CharacterAnimDataAsset->CharacterAnimationData.CrouchBlendSpace;
		}
	}
	return DefaultCharacterAnimDataAsset ? DefaultCharacterAnimDataAsset->CharacterAnimationData.CrouchBlendSpace : nullptr;
}

UAnimSequenceBase* UAG_AnimInstance::GetCrouchIdleAnimation() const
{
	if (const UItemStaticData* ItemData = GetEquippedItemData())
	{
		if (ItemData->CharacterAnimationData.CrouchIdleAnimationAsset)
		{
			return ItemData->CharacterAnimationData.CrouchIdleAnimationAsset;
		}
	}
	
	if (AAG_Character* ActionGameCharacter = Cast<AAG_Character>(GetOwningActor()))
	{
		FCharacterData Data = ActionGameCharacter->GetCharacterData();
		if (Data.CharacterAnimDataAsset)
		{
			return Data.CharacterAnimDataAsset->CharacterAnimationData.CrouchIdleAnimationAsset;
		}
	}
	return DefaultCharacterAnimDataAsset ? DefaultCharacterAnimDataAsset->CharacterAnimationData.CrouchIdleAnimationAsset : nullptr;
}
