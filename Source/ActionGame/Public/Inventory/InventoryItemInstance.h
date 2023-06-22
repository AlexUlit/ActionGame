// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActionGameTypes.h"
#include "ActiveGameplayEffectHandle.h"
#include "GameplayAbilitySpecHandle.h"
#include "InventoryItemInstance.generated.h"

class AItemActor;

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class ACTIONGAME_API UInventoryItemInstance : public UObject
{
	GENERATED_BODY()
	
public:
	virtual void Init(TSubclassOf<UItemStaticData> InItemStaticDataClass);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	const UItemStaticData* GetItemStaticData() const;
	
	virtual void OnEquipped(AActor* InOwner = nullptr);
	virtual void OnUnequipped(AActor* InOwner = nullptr);
	virtual void OnDropped(AActor* InOwner = nullptr);

	UFUNCTION(BlueprintPure)
	AItemActor* GetItemActor() const;

	UPROPERTY(Replicated)
	TSubclassOf<UItemStaticData> ItemStaticDataClass;

protected:
	void TryGrandAbilities(AActor* InOwner);
	void TryRemoveAbilities(AActor* InOwner);
	void TryApplyEffects(AActor* InOwner);
	void TryRemoveEffects(AActor* InOwner);
	
private:
	virtual bool IsSupportedForNetworking() const override {return true;}


	UFUNCTION()
	void OnRep_Equipped();
	
private:
	
	UPROPERTY(ReplicatedUsing = OnRep_Equipped)
	bool bEquipped = false;

protected:
	UPROPERTY(Replicated)
	AItemActor* ItemActor = nullptr;

	UPROPERTY()
	TArray<FGameplayAbilitySpecHandle> GrantedAbilityHandles;

	TArray<FActiveGameplayEffectHandle> OngoingEffectHandles;
};
