// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/GA_WallRun.h"

#include "AbilitySystem/AbilityTasks/AbilityTask_TickWallRun.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/AG_Character.h"


UGA_WallRun::UGA_WallRun()
{
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UGA_WallRun::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);

	ACharacter* CharacterAvatar = Cast<ACharacter>(ActorInfo->AvatarActor);
	UCapsuleComponent* CapsuleComponent = CharacterAvatar->GetCapsuleComponent();

	CapsuleComponent->OnComponentHit.AddDynamic(this, &UGA_WallRun::OnCapsuleComponentHit);
	
}

void UGA_WallRun::OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	if (ActorInfo)
	{
		ACharacter* CharacterAvatar = Cast<ACharacter>(ActorInfo->AvatarActor);
		UCapsuleComponent* CapsuleComponent = CharacterAvatar->GetCapsuleComponent();

		CapsuleComponent->OnComponentHit.RemoveDynamic(this, &UGA_WallRun::OnCapsuleComponentHit);
	}
	Super::OnRemoveAbility(ActorInfo, Spec);
}

bool UGA_WallRun::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags,
	const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}
	
	AAG_Character* CharacterAvatar = Cast<AAG_Character>(ActorInfo->AvatarActor);
	
	return CharacterAvatar && !CharacterAvatar->GetCharacterMovement()->IsMovingOnGround();
}

void UGA_WallRun::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	WallRunTickTask = UAbilityTask_TickWallRun::CreateWallRunTask(this,Cast<ACharacter>(ActorInfo->AvatarActor), Cast<UCharacterMovementComponent>(ActorInfo->MovementComponent), WallRun_TraceObjectTypes);
	WallRunTickTask->OnFinished.AddDynamic(this, &UGA_WallRun::K2_EndAbility);
	WallRunTickTask->OnWallSideDetermined.AddDynamic(this, &UGA_WallRun::OnWallSideDetermened);
	WallRunTickTask->ReadyForActivation();
}

void UGA_WallRun::OnCapsuleComponentHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
                                        UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (ASC)
	{
		ASC->TryActivateAbility(GetCurrentAbilitySpecHandle()); //GetCurrentAbilitySpec()->Handle
	}
}

void UGA_WallRun::OnWallSideDetermened(bool bLeftSide)
{
	AAG_Character* CharacterAvatar = GetActionGameCharacterFromActorInfo();
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	FGameplayEffectContextHandle EffectContextHandle = ASC->MakeEffectContext();
	
	if (bLeftSide)
	{
		CharacterAvatar->ApplyGameplayEffectToSelf(WallRunLeftSideEffectClass, EffectContextHandle);
	}
	else
	{
		CharacterAvatar->ApplyGameplayEffectToSelf(WallRunRightSideEffectClass, EffectContextHandle);
	}
}

void UGA_WallRun::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                             const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (IsValid(WallRunTickTask))
	{
		WallRunTickTask->EndTask();
	}

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (ASC)
	{
		ASC->RemoveActiveGameplayEffectBySourceEffect(WallRunLeftSideEffectClass, ASC);
		ASC->RemoveActiveGameplayEffectBySourceEffect(WallRunRightSideEffectClass, ASC);
	}
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}




