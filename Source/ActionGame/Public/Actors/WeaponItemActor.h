// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/ItemActor.h"
#include "WeaponItemActor.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONGAME_API AWeaponItemActor : public AItemActor
{
	GENERATED_BODY()

public:
	AWeaponItemActor();

	const UWeaponStaticData* GetWeaponStaticData() const;

	UFUNCTION(BlueprintPure)
	FVector GetMuzzleLocation() const;

	UFUNCTION(BlueprintCallable)
	void PlayerWeaponEffects(const FHitResult& InHitResult);

	

protected:
	virtual void InitInternal() override;

	UFUNCTION(NetMulticast, Reliable)
	void MulticastPlayerWeaponEffects(const FHitResult& InHitResult);

	void PlayerWeaponEffectsInternal(const FHitResult& InHitResult);
	
protected:
	UPROPERTY()
	UMeshComponent* MeshComponent = nullptr;
};
