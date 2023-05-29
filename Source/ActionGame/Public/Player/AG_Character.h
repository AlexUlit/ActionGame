﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "ActionGameTypes.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Abilities/GameplayAbility.h"
#include "AG_Character.generated.h"



class UCameraComponent;
class USpringArmComponent;
class UAG_InputConfigData;
class UInputMappingContext;
class UAG_AbilitySystemComponentBase;
class UAG_AttributeSet;
class UGameplayEffect;

UCLASS(Config=Game)
class AAG_Character : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AAG_Character();
	
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void PostInitializeComponents() override;
	
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
	bool ApplyGameplayEffectToSelf(TSubclassOf<UGameplayEffect> Effect, FGameplayEffectContextHandle InEffectContext);

	UFUNCTION(BlueprintCallable)
	FCharacterData GetCharacterData() const;

	UFUNCTION(BlueprintCallable)
	void SetCharacterData(const FCharacterData& InCharacterData);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void GiveAbilities();
	void ApplyStartupEffects();

	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;
	virtual void InitFromCharacterData(const FCharacterData& InCharacterData, bool bFromReplication = false);
	
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);

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
	UAG_AbilitySystemComponentBase* AbilitySystemComponent;

	UPROPERTY(Transient)
	UAG_AttributeSet* AttributeSet;

	UPROPERTY(ReplicatedUsing = OnRep_CharacterData)
	FCharacterData CharacterData;

	UPROPERTY(EditDefaultsOnly)
	class UCharacterDataAsset* CharacterDataAsset;
};
