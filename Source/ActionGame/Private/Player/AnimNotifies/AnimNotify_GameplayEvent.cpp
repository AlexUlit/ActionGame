// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/AnimNotifies/AnimNotify_GameplayEvent.h"

#include "AbilitySystemBlueprintLibrary.h"

void UAnimNotify_GameplayEvent::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(MeshComp->GetOwner(), PayLoad.EventTag, PayLoad);
}
