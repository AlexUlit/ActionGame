// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/Components/FootstepsComponent.h"

#include "Kismet/GameplayStatics.h"
#include "PhysicalMaterials/AG_PhysicalMaterial.h"
#include "Player/AG_Character.h"
#include "DrawDebugHelpers.h"

static TAutoConsoleVariable<int32> CVarShowFootsteps(
	TEXT("ShowDebugFootsteps"),
	0,
	TEXT("Draws debug into about footsteps")
	TEXT(" 0: off/n")
	TEXT(" 1: on/n"),
	ECVF_Cheat);

// Sets default values for this component's properties
UFootstepsComponent::UFootstepsComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UFootstepsComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

void UFootstepsComponent::HandleFootstep(EFoot Foot)
{
	if(AAG_Character* Character = Cast<AAG_Character>(GetOwner()))
	{
		const int32 DebugShowFootsteps = CVarShowFootsteps.GetValueOnAnyThread();
		
		if(USkeletalMeshComponent* Mesh = Character->GetMesh())
		{
			FHitResult HitResult;
			const FVector SocketLocation = Mesh->GetSocketLocation(Foot == EFoot::Left ? LeftFootSocketName : RightFootSocketName);
			const FVector Location = SocketLocation + FVector::UpVector * 20.f;
			const FVector EndLocation = SocketLocation + FVector::UpVector * -50.f;

			FCollisionQueryParams QueryParams;
			QueryParams.bReturnPhysicalMaterial = true;
			QueryParams.AddIgnoredActor(Character);

			if(GetWorld()->LineTraceSingleByChannel(HitResult, Location, EndLocation, ECC_WorldStatic, QueryParams))
			{
				if(HitResult.bBlockingHit && HitResult.PhysMaterial.Get())
				{
					UAG_PhysicalMaterial* PhysicalMaterial = Cast<UAG_PhysicalMaterial>(HitResult.PhysMaterial.Get());
					if (PhysicalMaterial)
					{
						UGameplayStatics::PlaySoundAtLocation(this, PhysicalMaterial->FootstepSound, EndLocation);
					}

					if (DebugShowFootsteps > 0)
					{
						DrawDebugString(GetWorld(),Location,GetNameSafe(PhysicalMaterial),nullptr, FColor::White, 4.f);
						DrawDebugSphere(GetWorld(), Location, 16, 16, FColor::Red, false, 4.f);
					}
				}
				else if (DebugShowFootsteps>0)
				{
					DrawDebugLine(GetWorld(),Location, EndLocation, FColor::Red, false, 4,0, 1);
				}
			}
		}
	}
}

