// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/AnimNotifies/AnimNotify_Step.h"

#include "Player/AG_Character.h"
#include "Player/Components/FootstepsComponent.h"

void UAnimNotify_Step::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

	check(MeshComp)
	
	AAG_Character* Character = MeshComp ? Cast<AAG_Character>(MeshComp->GetOwner()) : nullptr;
	if (Character)
	{
		if (UFootstepsComponent* FootstepsComponent = Cast<UFootstepsComponent>(Character->GetFootstepsComponent()))
		{
			FootstepsComponent->HandleFootstep(Foot);
		}
		
	}
}
