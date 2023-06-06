// Fill out your copyright notice in the Description page of Project Settings.

#include "Player/AnimInstances/AG_AnimInstance.h"
#include "ActionGame/DataAssets/CharacterAnimDataAsset.h"
#include "Player/AG_Character.h"



UBlendSpace* UAG_AnimInstance::GetLocomotionBlendSpace() const
{
	if (AAG_Character* ActionGameCharacter = Cast<AAG_Character>(GetOwningActor()))
	{
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
