// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActionGameTypes.h"
#include "InventoryItemInstance.generated.h"

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
	virtual void OnUnequipped();
	virtual void OnDropped();

	UPROPERTY(Replicated)
	TSubclassOf<UItemStaticData> ItemStaticDataClass;
	
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
};
