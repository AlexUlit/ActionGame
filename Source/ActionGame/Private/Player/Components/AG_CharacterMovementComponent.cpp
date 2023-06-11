// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/Components/AG_CharacterMovementComponent.h"

#include "AbilitySystemComponent.h"
#include "Abilities/GameplayAbility.h"

static TAutoConsoleVariable<int32> CVarShowTraversal(
	TEXT("ShowDebugTraversal"),
	0,
	TEXT("Draw debug info about traversal")
	TEXT(" 0: off/n")
	TEXT(" 1: on/n"),
	ECVF_Cheat);



bool UAG_CharacterMovementComponent::TryTraversal(UAbilitySystemComponent* ASC)
{
	for(TSubclassOf<UGameplayAbility> AbilityClass : TraversalAbilitiesOrdered)
	{
		if(ASC->TryActivateAbilityByClass(AbilityClass, true))
		{
			FGameplayAbilitySpec* Spec;
			Spec = ASC->FindAbilitySpecFromClass(AbilityClass);
			if (Spec && Spec->IsActive())
			{
				return true;
			}
		}
	}
	return false;
}
