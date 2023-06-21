// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "ActionGameTypes.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Abilities/GameplayAbility.h"
#include "AG_Character.generated.h"


class UInventoryComponent;
class UAG_CharacterMovementComponent;
class UFootstepsComponent;
class UCameraComponent;
class USpringArmComponent;
class UAG_InputConfigData;
class UInputMappingContext;
class UAG_AbilitySystemComponentBase;
class UAG_AttributeSet;
class UGameplayEffect;
class UAG_MotionWarpingComponent;

UCLASS(Config=Game)
class AAG_Character : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AAG_Character(const FObjectInitializer& ObjectInitializer);
	
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void PostInitializeComponents() override;
	
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
	bool ApplyGameplayEffectToSelf(TSubclassOf<UGameplayEffect> Effect, FGameplayEffectContextHandle InEffectContext);

	UFUNCTION(BlueprintCallable)
	FCharacterData GetCharacterData() const;

	UFUNCTION(BlueprintCallable)
	void SetCharacterData(const FCharacterData& InCharacterData);

	UFUNCTION()
	UInventoryComponent* GetInventoryComponent() const;

	UFUNCTION()
	UFootstepsComponent* GetFootstepsComponent() const;

	UFUNCTION()
	UAG_MotionWarpingComponent* GetMotionWarpingComponent() const;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void GiveAbilities();
	void ApplyStartupEffects();

	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;
	virtual void InitFromCharacterData(const FCharacterData& InCharacterData, bool bFromReplication = false);

	virtual void OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;
	virtual void OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;

	void OnMaxMovementSpeedChanged(const FOnAttributeChangeData& Data);
	
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void ActivateJump();
	void ActivateCrouch();
	void DeactivateCrouch();
	void ActivateSprint();
	void DeactivateSprint();
	void DropItem();
	void EquipNextItem();
	void UnequipItem();
	void ActivateAttack();
	void DeactivateAttack();
	
	virtual void Landed(const FHitResult& Hit) override;

	UFUNCTION()
	void OnRep_CharacterData();

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Input")
	UInputMappingContext* InputMapping;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	UAG_InputConfigData* InputActions;

	UPROPERTY(EditDefaultsOnly)
	UAG_AbilitySystemComponentBase* ASC;

	UPROPERTY(Transient)
	UAG_AttributeSet* AttributeSet;

	UPROPERTY(ReplicatedUsing = OnRep_CharacterData)
	FCharacterData CharacterData;

	UPROPERTY(EditDefaultsOnly)
	class UCharacterDataAsset* CharacterDataAsset;

	UAG_CharacterMovementComponent* AGCharacterMovementComponent;

	UPROPERTY()
	UFootstepsComponent* FootstepsComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MotionWarp")
	UAG_MotionWarpingComponent* MotionWarpingComponent;

	UPROPERTY(EditAnywhere, Replicated)
	UInventoryComponent* InventoryComponent = nullptr;

	// Gameplay Events
	UPROPERTY(EditDefaultsOnly, Category = "GameplayEvents")
	FGameplayTag JumpEventTag;

	UPROPERTY(EditDefaultsOnly, Category = "GameplayEvents")
	FGameplayTag AttackStartedEventTag;

	UPROPERTY(EditDefaultsOnly, Category = "GameplayEvents")
	FGameplayTag AttackEndedEventTag;

	// Gameplay Tags
	UPROPERTY(EditDefaultsOnly, Category = "GameplayTags")
	FGameplayTagContainer InAirTag;

	UPROPERTY(EditDefaultsOnly, Category = "GameplayTags")
	FGameplayTagContainer CrouchTag;

	UPROPERTY(EditDefaultsOnly, Category ="GameplayTags")
	FGameplayTagContainer SprintTag;

	// Gameplay Effects
	UPROPERTY(EditDefaultsOnly, Category = "GameplayEffects")
	TSubclassOf<UGameplayEffect> CrouchStateEffect;

	//Delegates
	FDelegateHandle MaxMovementSpeedChangedDelegateHandle;
};
