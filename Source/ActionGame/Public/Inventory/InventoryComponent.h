// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "InventoryList.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ACTIONGAME_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UInventoryComponent();
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void InitializeComponent() override;
	virtual bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
	
	virtual void GameplayEventCallback(const FGameplayEventData* Payload);

	UFUNCTION(BlueprintCallable)
	void AddItem(TSubclassOf<UItemStaticData> InItemStaticDataClass);

	UFUNCTION(BlueprintCallable)
	void AddItemInstance(UInventoryItemInstance* InItemInstance);

	UFUNCTION(BlueprintCallable)
	void RemoveItem(TSubclassOf<UItemStaticData> InItemStaticDataClass);

	UFUNCTION(BlueprintCallable)
	void EquipItem(TSubclassOf<UItemStaticData> InItemStaticDataClass);

	UFUNCTION(BlueprintCallable)
	void EquipItemInstance(UInventoryItemInstance* InItemInstance);

	UFUNCTION(BlueprintCallable)
	void UnequipItem();

	UFUNCTION(BlueprintCallable)
	void DropItem();

	UFUNCTION(BlueprintCallable)
	void EquipNext();
	
	UFUNCTION(BlueprintCallable)
	UInventoryItemInstance* GetEquippedItem() const;

	static FGameplayTag EquipItemActorTag;
	static FGameplayTag DropItemTag;
	static FGameplayTag EquipNextTag;
	static FGameplayTag UnequipTag;
	
protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void AddInventoryTags();

	void HandleGameplayEventInternal(FGameplayEventData Payload);

	UFUNCTION(Server, Reliable)
	void ServerHandleGameplayEvent(FGameplayEventData Paylod);
protected:
	UPROPERTY(Replicated)
	FInventoryList InventoryList;
	
	UPROPERTY(EditDefaultsOnly)
	TArray<TSubclassOf<UItemStaticData>> DefaultItems;

	UPROPERTY(Replicated)
	UInventoryItemInstance* CurrentItem = nullptr;

	FDelegateHandle TagDelegateHandle;
};
