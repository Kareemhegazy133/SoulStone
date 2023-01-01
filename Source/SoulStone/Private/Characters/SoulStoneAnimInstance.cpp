// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/SoulStoneAnimInstance.h"
#include "Characters/SoulStoneCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void USoulStoneAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	SoulStoneCharacter = Cast<ASoulStoneCharacter>(TryGetPawnOwner());
	if (SoulStoneCharacter) {
		SoulStoneCharacterMovement = SoulStoneCharacter->GetCharacterMovement();
	}

}

void USoulStoneAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	if (SoulStoneCharacterMovement) {
		GroundSpeed = UKismetMathLibrary::VSizeXY(SoulStoneCharacterMovement->Velocity);
		IsFalling = SoulStoneCharacterMovement->IsFalling();
		CharacterState = SoulStoneCharacter->GetCharacterState();
	}
}
