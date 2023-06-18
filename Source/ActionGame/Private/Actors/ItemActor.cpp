// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/ItemActor.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Components/SphereComponent.h"
#include "Engine/ActorChannel.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Inventory/InventoryItemInstance.h"


// Sets default values
AItemActor::AItemActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	SphereComponent->SetupAttachment(RootComponent);
	SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &AItemActor::OnSphereOverlap);
}

void AItemActor::Init(UInventoryItemInstance* InInstance)
{
	ItemInstance = InInstance;
}

void AItemActor::BeginPlay()
{
	Super::BeginPlay();
}

void AItemActor::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	FGameplayEventData EventPayload;
	EventPayload.OptionalObject = this;
	
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(OtherActor, OverlapEventTag, EventPayload);
}


void AItemActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

bool AItemActor::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool WroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);
	WroteSomething |= Channel->ReplicateSubobject(Channel, *Bunch, *RepFlags);
	return WroteSomething;
}

void AItemActor::OnEquipped()
{
	ItemState = EItemState::Equipped;
	SphereComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AItemActor::OnUnequipped()
{
	ItemState = EItemState::None;
	SphereComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AItemActor::OnDropped()
{
	ItemState = EItemState::Dropped;
	SphereComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
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
	
	 	EDrawDebugTrace::Type DebugDrawType = bShowDebugInventory ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None;
	
	 	if (UKismetSystemLibrary::LineTraceSingleByProfile(this, TraceStart, TraceEnd, TEXT("WorldStatic"), true, ActorsToIgnore, DebugDrawType, TraceHit, true))
	 	{
	 		if (TraceHit.bBlockingHit)
	 		{
	 			SetActorLocation(TraceHit.Location);
	 			return;
	 		}
	 	}
	 	SetActorLocation(TraceEnd);
	}
}

void AItemActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AItemActor, ItemInstance);
	DOREPLIFETIME(AItemActor, ItemState);
}

