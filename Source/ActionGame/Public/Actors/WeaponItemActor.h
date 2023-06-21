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

	AWeaponItemActor();
	
	const UWeaponStaticData* GetWeaponStaticData() const;

	UFUNCTION(BlueprintPure)
	FVector GetMuzzleLocation() const;

protected:
	virtual void InitInternal() override;
	
protected:
	UPROPERTY()
	UMeshComponent* MeshComponent = nullptr;
};
