// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActionGameTypes.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

class UProjectileMovementComponent;

UCLASS()
class ACTIONGAME_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	AProjectile();
	
	const UProjectileStaticData* GetProjectileStaticData() const ;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	void DebugDrawPath() const;

	UFUNCTION()
	void OnProjectileStop(const FHitResult& InpactResult);

public:
	UPROPERTY(BlueprintReadOnly, Replicated)
	TSubclassOf<UProjectileStaticData> ProjectileDataClass;

protected:
	UPROPERTY()
	UStaticMeshComponent* StaticMeshComponent = nullptr;
	
	UPROPERTY()
	UProjectileMovementComponent* ProjectileMovementComponent = nullptr;
};
