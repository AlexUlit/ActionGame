﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/AG_Character.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "AbilitySystem/AttributeSets/AG_AttributeSet.h"
#include "AbilitySystem/Components/AG_AbilitySystemComponentBase.h"
#include "ActionGame/DataAssets/CharacterDataAsset.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Player/Input/AG_InputConfigData.h"
#include "Net/UnrealNetwork.h"

//----------------------------------------------------------------------------------------------------------------------
AAG_Character::AAG_Character()
{// Sets default values
	PrimaryActorTick.bCanEverTick = false;

	//Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.f;
	CameraBoom->bUsePawnControlRotation = true;

	//Create a follow camera 
	FollowCamera =CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	//AbilitySystem
	AbilitySystemComponent = CreateDefaultSubobject<UAG_AbilitySystemComponentBase>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	AttributeSet = CreateDefaultSubobject<UAG_AttributeSet>(TEXT("AttributeSet"));
	
}

//----------------------------------------------------------------------------------------------------------------------
void AAG_Character::BeginPlay()
{// Called when the game starts or when spawned
	Super::BeginPlay();
}

void AAG_Character::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	AbilitySystemComponent->InitAbilityActorInfo(this, this);
	GiveAbilities();
	ApplyStartupEffects();
}

void AAG_Character::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
}

void AAG_Character::InitFromCharacterData(const FCharacterData& InCharacterData, bool bFromReplication)
{
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
}

void AAG_Character::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	if(IsValid(CharacterDataAsset))
	{
		SetCharacterData(CharacterDataAsset->CharacterData);
	}
}

UAbilitySystemComponent* AAG_Character::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
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

	FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(Effect, 1, InEffectContext);
	if (SpecHandle.IsValid())
	{
		FActiveGameplayEffectHandle ActiveGEHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
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

//----------------------------------------------------------------------------------------------------------------------
void AAG_Character::GiveAbilities()
{
	if (HasAuthority() && AbilitySystemComponent)
	{
		for (auto DefaultAbility : CharacterData.Abilities)
		{
			AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(DefaultAbility));
		}
	}
}

void AAG_Character::ApplyStartupEffects()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
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
}


