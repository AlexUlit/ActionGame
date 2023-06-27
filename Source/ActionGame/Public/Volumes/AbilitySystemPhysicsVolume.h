// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActiveGameplayEffectHandle.h"
#include "GameplayAbilitySpecHandle.h"
#include "GameplayTagContainer.h"
#include "GameFramework/PhysicsVolume.h"
#include "Interfaces/ITargetDevice.h"
#include "AbilitySystemPhysicsVolume.generated.h"

class UGameplayEffect;
class UGameplayAbility;

USTRUCT()
struct FAbilityVolumeEnteredActorInfo
{
	GENERATED_BODY()

	TArray<FGameplayAbilitySpecHandle> AppliesAbilities;
	TArray<FActiveGameplayEffectHandle> AppliedEffects;
	
};

UCLASS()
class ACTIONGAME_API AAbilitySystemPhysicsVolume : public APhysicsVolume
{
	GENERATED_BODY()

public:
	AAbilitySystemPhysicsVolume();
	
	virtual void ActorEnteredVolume(AActor* Other) override;
	virtual void ActorLeavingVolume(AActor* Other) override;
	virtual void Tick(float DeltaSeconds) override;
	
protected:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TSubclassOf<UGameplayEffect>> OngoingEffectsToApply;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TSubclassOf<UGameplayEffect>> OnExitEffectsToApply;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bDrawDebug = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FGameplayTag> GameplayEventsToSendOnEnter;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FGameplayTag> GameplayEventsToSendOnExit;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TSubclassOf<UGameplayAbility>> OngoingAbilitiesToGive;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TSubclassOf<UGameplayAbility>> PermanentAbilitiesToGive;

	TMap<AActor*, FAbilityVolumeEnteredActorInfo> EnteredActorsIntoMap;
	
};
