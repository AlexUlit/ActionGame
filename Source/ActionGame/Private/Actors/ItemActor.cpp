// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/ItemActor.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Components/SphereComponent.h"
#include "Engine/ActorChannel.h"
#include "Inventory/InventoryComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Inventory/InventoryItemInstance.h"


// Sets default values
AItemActor::AItemActor()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	SetReplicateMovement(true);

	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	SphereComponent->SetupAttachment(RootComponent);
	SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &AItemActor::OnSphereOverlap);
}

void AItemActor::Init(UInventoryItemInstance* InInstance)
{
	ItemInstance = InInstance;
	InitInternal();
}

void AItemActor::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		if (!IsValid(ItemInstance) && IsValid(ItemStaticDataClass))
		{
			ItemInstance = NewObject<UInventoryItemInstance>();
			ItemInstance->Init(ItemStaticDataClass);

			SphereComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
			SphereComponent->SetGenerateOverlapEvents(true);

			InitInternal();
		}
	}
}

void AItemActor::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (HasAuthority())
	{
		FGameplayEventData EventPayload;
		EventPayload.Instigator = this;
		EventPayload.OptionalObject = ItemInstance;
		EventPayload.EventTag = UInventoryComponent::EquipItemActorTag;
	
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(OtherActor, UInventoryComponent::EquipItemActorTag, EventPayload);
	}
	
}

void AItemActor::InitInternal()
{
	
}

void AItemActor::OnRep_ItemState()
{
	switch (ItemState)
	{
	case EItemState::Equipped:
		SphereComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		SphereComponent->SetGenerateOverlapEvents(false);
		break;
	default:
		SphereComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		SphereComponent->SetGenerateOverlapEvents(true);
		break;
	}
}

void AItemActor::OnRep_ItemInstance(UInventoryItemInstance* OldItemInstance)
{
	if (IsValid(ItemInstance) && !IsValid(OldItemInstance))
	{
		InitInternal();
	}
}


void AItemActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

bool AItemActor::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool WroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);
	WroteSomething |= Channel->ReplicateSubobject(ItemInstance, *Bunch, *RepFlags);
	return WroteSomething;
}

void AItemActor::OnEquipped()
{
	ItemState = EItemState::Equipped;
	
	SphereComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SphereComponent->SetGenerateOverlapEvents(false);
}

void AItemActor::OnUnequipped()
{
	ItemState = EItemState::None;
	
	SphereComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SphereComponent->SetGenerateOverlapEvents(false);
}

void AItemActor::OnDropped()
{
	ItemState = EItemState::Dropped;
	
	GetRootComponent()->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform );

	 if (AActor* ActorOwner = GetOwner())
	 {
	 	const FVector Location  = GetActorLocation();
	 	const FVector Forward = ActorOwner->GetActorForwardVector();
	
	 	const float DroppItemDistance = 100.f;
	 	const float DroppItemTraceDistance = 1000.f;
	
	 	const FVector TraceStart = Location + Forward * DroppItemDistance;
	 	const FVector TraceEnd = TraceStart - FVector::UpVector * DroppItemTraceDistance;
	 	
	 	TArray<AActor*> ActorsToIgnore = {GetOwner()};
	 	FHitResult TraceHit;
	
	 	static const auto CVar = IConsoleManager::Get().FindConsoleVariable(TEXT("ShowDebugInventory"));
	 	const bool bShowDebugInventory = CVar->GetInt()>0;

		FVector TargetLocation = TraceEnd;
	 	
	 	EDrawDebugTrace::Type DebugDrawType = bShowDebugInventory ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None;
	 	if (UKismetSystemLibrary::LineTraceSingleByProfile(this, TraceStart, TraceEnd, TEXT("WorldStatic"), true, ActorsToIgnore, DebugDrawType, TraceHit, true))
	 	{
	 		if (TraceHit.bBlockingHit)
	 		{
	 			TargetLocation = TraceHit.Location;
	 		}
	 	}
	 	SetActorLocation(TargetLocation);

	 	SphereComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	 	SphereComponent->SetGenerateOverlapEvents(true);
	}
}

void AItemActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AItemActor, ItemInstance);
	DOREPLIFETIME(AItemActor, ItemState);
}

