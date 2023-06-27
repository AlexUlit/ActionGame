// Fill out your copyright notice in the Description page of Project Settings.


#include "Volumes/AbilitySystemPhysicsVolume.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"

AAbilitySystemPhysicsVolume::AAbilitySystemPhysicsVolume()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AAbilitySystemPhysicsVolume::ActorEnteredVolume(AActor* Other)
{
	Super::ActorEnteredVolume(Other);

	if (!HasAuthority()) return;

	if (UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Other))
	{
		for (auto Ability : PermanentAbilitiesToGive)
		{
			ASC->GiveAbility(FGameplayAbilitySpec(Ability));
		}
		EnteredActorsIntoMap.Add(Other);

		for (auto Ability : OngoingAbilitiesToGive)
		{
			FGameplayAbilitySpecHandle AbilityHandle = ASC->GiveAbility(FGameplayAbilitySpec(Ability));
			EnteredActorsIntoMap[Other].AppliesAbilities.Add(AbilityHandle);
		}

		for (auto GameplayEffect : OngoingEffectsToApply)
		{
			FGameplayEffectContextHandle EffectContext = ASC->MakeEffectContext();
			
			EffectContext.AddInstigator(Other,Other);

			FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(GameplayEffect, 1, EffectContext);
			if (SpecHandle.IsValid())
			{
				FActiveGameplayEffectHandle ActiveGEHandle = ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
				if (ActiveGEHandle.WasSuccessfullyApplied())
				{
					EnteredActorsIntoMap[Other].AppliedEffects.Add(ActiveGEHandle);
				}
			}
		}
		for (auto EventTag : GameplayEventsToSendOnExit)
		{
			FGameplayEventData EventPayLoad;
			EventPayLoad.EventTag = EventTag;

			UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Other, EventTag, EventPayLoad);
		}
	}
}

void AAbilitySystemPhysicsVolume::ActorLeavingVolume(AActor* Other)
{
	Super::ActorLeavingVolume(Other);

	if (!HasAuthority()) return;
	if (UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Other))
	{
		if (EnteredActorsIntoMap.Find(Other))
		{
			for (auto GameplayEffectHandle : EnteredActorsIntoMap[Other].AppliedEffects)
			{
				ASC->RemoveActiveGameplayEffect(GameplayEffectHandle);
			}

			for (auto GameplayAbilityHandle : EnteredActorsIntoMap[Other].AppliesAbilities)
			{
				ASC->ClearAbility(GameplayAbilityHandle);
			}
			EnteredActorsIntoMap.Remove(Other);
		}
		for (auto GameplayEffect : OnExitEffectsToApply)
		{
			FGameplayEffectContextHandle EffectContext = ASC->MakeEffectContext();
			
			EffectContext.AddInstigator(Other,Other);

			FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(GameplayEffect, 1, EffectContext);
			if (SpecHandle.IsValid())
			{
				FActiveGameplayEffectHandle ActiveGEHandle = ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
			}
		}
		for (auto EventTag : GameplayEventsToSendOnExit)
		{
			FGameplayEventData EventPayLoad;
			EventPayLoad.EventTag = EventTag;

			UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Other, EventTag, EventPayLoad);
		}
	}
}

void AAbilitySystemPhysicsVolume::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (bDrawDebug)
	{
		DrawDebugBox(GetWorld(), GetActorLocation(), GetBounds().BoxExtent, FColor::Red, false, 0, 0, 5);
	}
}
