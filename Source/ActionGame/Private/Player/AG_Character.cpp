// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/AG_Character.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "AbilitySystem/AttributeSets/AG_AttributeSet.h"
#include "AbilitySystem/Components/AG_AbilitySystemComponentBase.h"
#include "ActionGame/DataAssets/CharacterDataAsset.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Inventory/InventoryComponent.h"
#include "Player/Input/AG_InputConfigData.h"
#include "Net/UnrealNetwork.h"
#include "Player/Components/AG_CharacterMovementComponent.h"
#include "Player/Components/AG_MotionWarpingComponent.h"
#include "Player/Components/FootstepsComponent.h"

//----------------------------------------------------------------------------------------------------------------------
AAG_Character::AAG_Character(const FObjectInitializer& ObjectInitializer):Super(ObjectInitializer.SetDefaultSubobjectClass<UAG_CharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	PrimaryActorTick.bCanEverTick = false;

	AGCharacterMovementComponent = Cast<UAG_CharacterMovementComponent>(GetCharacterMovement());

	//Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.f;
	CameraBoom->bUsePawnControlRotation = true;

	//Create a follow camera 
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	//AbilitySystem
	ASC = CreateDefaultSubobject<UAG_AbilitySystemComponentBase>(TEXT("AbilitySystemComponent"));
	ASC->SetIsReplicated(true);
	ASC->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
	
	AttributeSet = CreateDefaultSubobject<UAG_AttributeSet>(TEXT("AttributeSet"));
	ASC->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetMaxMovementSpeedAttribute()).AddUObject(this,&AAG_Character::OnMaxMovementSpeedChanged);

	FootstepsComponent = CreateDefaultSubobject<UFootstepsComponent>(TEXT("FootstepsComponent"));
	MotionWarpingComponent = CreateDefaultSubobject<UAG_MotionWarpingComponent>(TEXT("MotionWarpingComponent"));

	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent")); // include?
	InventoryComponent->SetIsReplicated(true);
}

//----------------------------------------------------------------------------------------------------------------------
void AAG_Character::BeginPlay()
{// Called when the game starts or when spawned
	Super::BeginPlay();
}

void AAG_Character::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	ASC->InitAbilityActorInfo(this, this);
	GiveAbilities();
	ApplyStartupEffects();
}

void AAG_Character::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	ASC->InitAbilityActorInfo(this, this);
}

void AAG_Character::InitFromCharacterData(const FCharacterData& InCharacterData, bool bFromReplication)
{
}

void AAG_Character::OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnStartCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
	if (!CrouchStateEffect.Get()) return;;
	UE_LOG(LogTemp, Error, TEXT("Crouch"));
	if (ASC)
	{
		FGameplayEffectContextHandle EffectContext = ASC->MakeEffectContext();
		FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(CrouchStateEffect, 1, EffectContext);

		if (SpecHandle.IsValid())
		{
			FActiveGameplayEffectHandle ActiveGEHandle = ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
			if (!ActiveGEHandle.WasSuccessfullyApplied())
			{
				UE_LOG(LogTemp, Error, TEXT("Ability %s failed to apply crouch effect %s"), *GetName(), * GetNameSafe(CrouchStateEffect));
			}
		}
	}
}

void AAG_Character::OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnEndCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);

	if (ASC && CrouchStateEffect.Get())
	{
		ASC->RemoveActiveGameplayEffectBySourceEffect(CrouchStateEffect, ASC);
	}
}

void AAG_Character::OnMaxMovementSpeedChanged(const FOnAttributeChangeData& Data)
{
	GetCharacterMovement()->MaxWalkSpeed = Data.NewValue;
}

//----------------------------------------------------------------------------------------------------------------------
void AAG_Character::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{// Called to bind functionality to input
	APlayerController* PC = Cast<APlayerController>(GetController());
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(
		PC->GetLocalPlayer());
	Subsystem->ClearAllMappings();
	Subsystem->AddMappingContext(InputMapping, 0);
	UEnhancedInputComponent* PEI = Cast<UEnhancedInputComponent>(PlayerInputComponent);

	PEI->BindAction(InputActions->InputMove, ETriggerEvent::Triggered, this, &AAG_Character::Move);
	PEI->BindAction(InputActions->InputLook, ETriggerEvent::Triggered, this, &AAG_Character::Look);
	PEI->BindAction(InputActions->Jump, ETriggerEvent::Started, this, &AAG_Character::ActivateJump);
	PEI->BindAction(InputActions->Crouch, ETriggerEvent::Started, this, &AAG_Character::ActivateCrouch);
	PEI->BindAction(InputActions->Crouch, ETriggerEvent::Completed, this, &AAG_Character::DeactivateCrouch);
	PEI->BindAction(InputActions->Sprint, ETriggerEvent::Started, this, &AAG_Character::ActivateSprint);
	PEI->BindAction(InputActions->Sprint, ETriggerEvent::Completed, this, &AAG_Character::DeactivateSprint);
	PEI->BindAction(InputActions->DropItem, ETriggerEvent::Triggered, this, &AAG_Character::DropItem);
	PEI->BindAction(InputActions->EquipNextItem, ETriggerEvent::Triggered, this, &AAG_Character::EquipNextItem);
	PEI->BindAction(InputActions->UnequipItem, ETriggerEvent::Triggered, this, &AAG_Character::UnequipItem);
	PEI->BindAction(InputActions->Attack, ETriggerEvent::Started, this, &AAG_Character::ActivateAttack);
	PEI->BindAction(InputActions->Attack, ETriggerEvent::Completed, this, &AAG_Character::DeactivateAttack);
}

void AAG_Character::PostLoad()
{
	Super::PostLoad();
	if(IsValid(CharacterDataAsset))
	{
		SetCharacterData(CharacterDataAsset->CharacterData);
	}
}

UAbilitySystemComponent* AAG_Character::GetAbilitySystemComponent() const
{
	return ASC;
}

//----------------------------------------------------------------------------------------------------------------------
void AAG_Character::Move(const FInputActionValue& Value)
{//Player Movement
	if (Controller != nullptr)
	{
		const FVector2d MoveValue = Value.Get<FVector2d>();
		const FRotator MovementRotation(0, Controller->GetControlRotation().Yaw,0);

		// Forward/Backward direction
		if (MoveValue.Y != 0.f)
		{
			const FVector Direction = MovementRotation.RotateVector(FVector::ForwardVector);
			AddMovementInput(Direction, MoveValue.Y);
		}
		
		// Right/Left direction
		if (MoveValue.X != 0.f)
		{
			const FVector Direction = MovementRotation.RotateVector(FVector::RightVector);
			AddMovementInput(Direction, MoveValue.X);
		}
	}
}

//----------------------------------------------------------------------------------------------------------------------
void AAG_Character::Look(const FInputActionValue& Value)
{//Player Look Around
	if(Controller != nullptr)
	{
		const FVector2d LookValue = Value.Get<FVector2D>();
		if (LookValue.X != 0.f)
		{
			AddControllerYawInput(LookValue.X);
		}

		if (LookValue.Y != 0.f)
		{
			AddControllerPitchInput(LookValue.Y);
		}
	}
}

void AAG_Character::ActivateJump()
{
	AGCharacterMovementComponent->TryTraversal(ASC);
}


void AAG_Character::ActivateCrouch()
{
	if (ASC)
	{
		ASC->TryActivateAbilitiesByTag(CrouchTag,true);
	}
}

void AAG_Character::DeactivateCrouch()
{
	if (ASC)
	{
		ASC->CancelAbilities(&CrouchTag);
	}
	
}

void AAG_Character::ActivateSprint()
{
	if (ASC)
	{
		ASC->TryActivateAbilitiesByTag(SprintTag, true);
	}
}

void AAG_Character::DeactivateSprint()
{
	if (ASC)
	{
		ASC->CancelAbilities(&SprintTag);
	}
}

void AAG_Character::DropItem()
{
	FGameplayEventData EventPayLoad;
	EventPayLoad.EventTag = UInventoryComponent::DropItemTag;
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, UInventoryComponent::DropItemTag, EventPayLoad);
}

void AAG_Character::EquipNextItem()
{
	FGameplayEventData EventPayLoad;
	EventPayLoad.EventTag = UInventoryComponent::EquipNextTag;
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, UInventoryComponent::EquipNextTag, EventPayLoad);
}

void AAG_Character::UnequipItem()
{
	FGameplayEventData EventPayLoad;
	EventPayLoad.EventTag = UInventoryComponent::UnequipTag;
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, UInventoryComponent::UnequipTag, EventPayLoad);
}

void AAG_Character::ActivateAttack()
{
	FGameplayEventData EventPayLoad;
	EventPayLoad.EventTag = AttackStartedEventTag;
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, AttackStartedEventTag, EventPayLoad);
}

void AAG_Character::DeactivateAttack()
{
	FGameplayEventData EventPayLoad;
	EventPayLoad.EventTag = AttackEndedEventTag;
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, AttackEndedEventTag, EventPayLoad);
}

void AAG_Character::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);
	if (ASC)
	{
		ASC->RemoveActiveEffectsWithTags(InAirTag);
	}
}

//----------------------------------------------------------------------------------------------------------------------
void AAG_Character::OnRep_CharacterData()
{
	InitFromCharacterData(CharacterData, true);
}

//----------------------------------------------------------------------------------------------------------------------

bool AAG_Character::ApplyGameplayEffectToSelf(TSubclassOf<UGameplayEffect> Effect,
	FGameplayEffectContextHandle InEffectContext)
{
	if (!Effect.Get())
	{
		return false;
	}

	FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(Effect, 1, InEffectContext);
	if (SpecHandle.IsValid())
	{
		FActiveGameplayEffectHandle ActiveGEHandle = ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		return ActiveGEHandle.WasSuccessfullyApplied();
	}
	return false;
}

FCharacterData AAG_Character::GetCharacterData() const
{
	return CharacterData;
}

void AAG_Character::SetCharacterData(const FCharacterData& InCharacterData)
{
	CharacterData = InCharacterData;
	InitFromCharacterData(CharacterData);
}

UInventoryComponent* AAG_Character::GetInventoryComponent() const
{
	return InventoryComponent;
}

UFootstepsComponent* AAG_Character::GetFootstepsComponent() const
{
	return FootstepsComponent;
}

UAG_MotionWarpingComponent* AAG_Character::GetMotionWarpingComponent() const
{
	return MotionWarpingComponent;
}

//----------------------------------------------------------------------------------------------------------------------
void AAG_Character::GiveAbilities()
{
	if (HasAuthority() && ASC)
	{
		for (auto DefaultAbility : CharacterData.Abilities)
		{
			ASC->GiveAbility(FGameplayAbilitySpec(DefaultAbility));
		}
	}
}

void AAG_Character::ApplyStartupEffects()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		FGameplayEffectContextHandle EffectContext = ASC->MakeEffectContext();
		EffectContext.AddSourceObject(this);

		for (auto CharacterEffect : CharacterData.Effects)
		{
			ApplyGameplayEffectToSelf(CharacterEffect,EffectContext);
		}
	}
}

void AAG_Character::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AAG_Character, CharacterData);
	DOREPLIFETIME(AAG_Character, InventoryComponent);
}



