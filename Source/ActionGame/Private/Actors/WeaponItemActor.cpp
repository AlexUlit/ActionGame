// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/WeaponItemActor.h"

#include "NiagaraFunctionLibrary.h"
#include "Inventory/InventoryItemInstance.h"
#include "Kismet/GameplayStatics.h"
#include "PhysicalMaterials/AG_PhysicalMaterial.h"

AWeaponItemActor::AWeaponItemActor()
{
}

const UWeaponStaticData* AWeaponItemActor::GetWeaponStaticData() const
{
	return ItemInstance ? Cast<UWeaponStaticData>(ItemInstance->GetItemStaticData()) : nullptr;
}

FVector AWeaponItemActor::GetMuzzleLocation() const
{
	return MeshComponent ? MeshComponent->GetSocketLocation(TEXT("Muzzle")) : GetActorLocation();
}

void AWeaponItemActor::PlayerWeaponEffects(const FHitResult& InHitResult)
{
	if (HasAuthority())
	{
		MulticastPlayerWeaponEffects(InHitResult);
	}
	else
	{
		PlayWeaponEffectsInternal(InHitResult);
	}
}

void AWeaponItemActor::InitInternal()
{
	Super::InitInternal();

	if (const UWeaponStaticData* WeaponData = GetWeaponStaticData())
	{
		if (WeaponData->SkeletalMesh)
		{
			USkeletalMeshComponent* SkeletalComp  = NewObject<USkeletalMeshComponent>(this, USkeletalMeshComponent::StaticClass(), TEXT("MeshComponent"));
			if (SkeletalComp)
			{
				SkeletalComp->RegisterComponent();
				SkeletalComp->SetSkeletalMesh(WeaponData->SkeletalMesh);
				SkeletalComp->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);

				MeshComponent = SkeletalComp;
			}
		}
		else if (WeaponData->StaticMesh)
		{
			UStaticMeshComponent* StaticComp = NewObject<UStaticMeshComponent>(this, UStaticMeshComponent::StaticClass(), TEXT("MeshComponent"));
			StaticComp->RegisterComponent();
			StaticComp->SetStaticMesh(WeaponData->StaticMesh);
			StaticComp->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);

			MeshComponent = StaticComp;
		}
	}
}

void AWeaponItemActor::PlayWeaponEffectsInternal(const FHitResult& InHitResult)
{
	if (InHitResult.PhysMaterial.Get())
	{
		UAG_PhysicalMaterial* PhysicalMaterial = Cast<UAG_PhysicalMaterial>(InHitResult.PhysMaterial.Get());
		if (PhysicalMaterial)
		{
			UGameplayStatics::PlaySoundAtLocation(this,PhysicalMaterial->PointImpactSound, InHitResult.ImpactPoint, 1.f);
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, PhysicalMaterial->PointImpactVFX, InHitResult.ImpactPoint);
		}
	}
	if (const UWeaponStaticData* WeaponData = GetWeaponStaticData())
	{
		UGameplayStatics::PlaySoundAtLocation(this, WeaponData->AttackSound, GetActorLocation(), 1.f);
	}
}

void AWeaponItemActor::MulticastPlayerWeaponEffects_Implementation(const FHitResult& InHitResult)
{
	if (!Owner || Owner->GetLocalRole() != ROLE_AutonomousProxy)
	{
		PlayWeaponEffectsInternal(InHitResult);
	}
}

