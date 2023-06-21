// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/WeaponItemActor.h"

#include "Inventory/InventoryItemInstance.h"

AWeaponItemActor::AWeaponItemActor()
{
}

const UWeaponStaticData* AWeaponItemActor::GetWeaponStaticData() const
{
	return ItemInstance ? Cast<UWeaponStaticData>(ItemInstance->GetItemStaticData()) : nullptr;
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
