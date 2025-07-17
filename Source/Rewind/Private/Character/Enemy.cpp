// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Enemy.h"
#include "GameFramework/CharacterMovementComponent.h"

AEnemy::AEnemy()
{
	GetCharacterMovement()->bOrientRotationToMovement = true;
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;
}
