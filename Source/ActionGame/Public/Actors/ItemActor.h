// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActionGameTypes.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Actor.h"
#include "ItemActor.generated.h"

class USphereComponent;
class UInventoryItemInstance;

UCLASS()
class ACTIONGAME_API AItemActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AItemActor();
	
	virtual void Tick(float DeltaTime) override;

	void Init(UInventoryItemInstance* InInstance);

	virtual bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
	
	virtual void OnEquipped();
	virtual void OnUnequipped();
	virtual void OnDropped();
	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

protected:
	UPROPERTY(Replicated)
	UInventoryItemInstance* ItemInstance = nullptr;

	UPROPERTY(Replicated)
	TEnumAsByte<EItemState> ItemState = EItemState::None;

	UPROPERTY()
	USphereComponent* SphereComponent = nullptr;
	
	FGameplayTag OverlapEventTag;
};
