// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/GA_InventoryCombatAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "ActionGameTypes.h"
#include "Actors/WeaponItemActor.h"
#include "Camera/CameraComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Player/AG_Character.h"

FGameplayEffectSpecHandle UGA_InventoryCombatAbility::GetWeaponEffectSpec(const FHitResult& InHitResult)
{
	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
	{
		if (const UWeaponStaticData* WeaponStaticData = GetEquippedItemWeaponStaticData())
		{
			FGameplayEffectContextHandle EffectContext = ASC->MakeEffectContext();
			FGameplayEffectSpecHandle OutSpec = ASC->MakeOutgoingSpec(WeaponStaticData->DamageEffect, 1, EffectContext);
			UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(OutSpec, FGameplayTag::RequestGameplayTag(TEXT("Attribute.Health")), -WeaponStaticData->BaseDamage);
			return OutSpec;
		}
	}
	return FGameplayEffectSpecHandle();
}

const bool UGA_InventoryCombatAbility::GetWeaponToFocusTraceResult(float TraceDistance, ETraceTypeQuery TraceType,
	FHitResult& OutHitResult)
{
	AWeaponItemActor* WeaponItemActor = GetEquippedWeaponItemActor();
	AAG_Character* ActionGameCharacter = GetActionGameCharacterFromActorInfo();

	const FTransform& CameraTransform = ActionGameCharacter->GetFollowCamera()->GetComponentTransform();
	const FVector FocusTraceEnd = CameraTransform.GetLocation() + CameraTransform.GetRotation().Vector() * TraceDistance;
	TArray<AActor*> ActorsToIgnore = {GetAvatarActorFromActorInfo()};
	FHitResult FocusHit;

	UKismetSystemLibrary::LineTraceSingle(this, CameraTransform.GetLocation(), FocusTraceEnd, TraceType, false, ActorsToIgnore, EDrawDebugTrace::None, FocusHit,true);

	FVector MuzzleLocation = WeaponItemActor->GetMuzzleLocation();
	const FVector WeaponTraceEnd = MuzzleLocation + (FocusHit.Location - MuzzleLocation).GetSafeNormal() * TraceDistance;

	UKismetSystemLibrary::LineTraceSingle(this, MuzzleLocation, WeaponTraceEnd, TraceType, false, ActorsToIgnore, EDrawDebugTrace::None, OutHitResult,true);

	return OutHitResult.bBlockingHit;
}
