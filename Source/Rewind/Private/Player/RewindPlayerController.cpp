// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/RewindPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Game/RewindGameModeBase.h"
#include "GameFramework/Character.h"
#include "Rewind/CharacterRewindComponent.h"
#include "Character/RewindCharacter.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

void ARewindPlayerController::BeginPlay()
{
	Super::BeginPlay();
	check(RewindMappingContext);

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(RewindMappingContext, 0);
	}

	GameMode = Cast<ARewindGameModeBase>(GetWorld()->GetAuthGameMode());

	CharacterRewindComponent = GetPawn()->FindComponentByClass<UCharacterRewindComponent>();
	check(CharacterRewindComponent);

	RewindCharacter = CastChecked<ARewindCharacter>(GetPawn());
	check(RewindCharacter);
}

void ARewindPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent))
	{
		/** Move */
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ARewindPlayerController::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ARewindPlayerController::StopJumping);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ARewindPlayerController::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ARewindPlayerController::Look);

		/** Time */
		EnhancedInputComponent->BindAction(RewindAction, ETriggerEvent::Started, this, &ARewindPlayerController::Rewind);
		EnhancedInputComponent->BindAction(RewindAction, ETriggerEvent::Completed, this, &ARewindPlayerController::StopRewind);
		EnhancedInputComponent->BindAction(FastForwardAction, ETriggerEvent::Started, this, &ARewindPlayerController::FastForward);
		EnhancedInputComponent->BindAction(FastForwardAction, ETriggerEvent::Completed, this, &ARewindPlayerController::StopFastForward);
		EnhancedInputComponent->BindAction(ToggleTimeScrubAction, ETriggerEvent::Started, this, &ARewindPlayerController::ToggleTimeScrub);
		EnhancedInputComponent->BindAction(ToggleRewindPartipationAction, ETriggerEvent::Started, this, &ARewindPlayerController::ToggleRewindParticipation);

		EnhancedInputComponent->BindAction(SetRewindSpeedSlowestAction, ETriggerEvent::Started, this, &ARewindPlayerController::SetRewindSpeedSlowest);
		EnhancedInputComponent->BindAction(SetRewindSpeedSlowerAction, ETriggerEvent::Started, this, &ARewindPlayerController::SetRewindSpeedSlower);
		EnhancedInputComponent->BindAction(SetRewindSpeedNormalAction, ETriggerEvent::Started, this, &ARewindPlayerController::SetRewindSpeedNormal);
		EnhancedInputComponent->BindAction(SetRewindSpeedFasterAction, ETriggerEvent::Started, this, &ARewindPlayerController::SetRewindSpeedFaster);
		EnhancedInputComponent->BindAction(SetRewindSpeedFastestAction, ETriggerEvent::Started, this, &ARewindPlayerController::SetRewindSpeedFastest);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void ARewindPlayerController::Move(const FInputActionValue& Value)
{
	// Ignore input while manipulating time
	if (CharacterRewindComponent->IsTimeBeingManipulated() || RewindCharacter->bIsDead) { return; }

	FVector2D MovementVector = Value.Get<FVector2D>();
	const FRotator Rotation = GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	if (RewindCharacter)
	{
		RewindCharacter->AddMovementInput(ForwardDirection, MovementVector.Y);
		RewindCharacter->AddMovementInput(RightDirection, MovementVector.X);
	}
}

void ARewindPlayerController::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (RewindCharacter)
	{
		RewindCharacter->AddControllerYawInput(LookAxisVector.X);
		RewindCharacter->AddControllerPitchInput(LookAxisVector.Y);
	}
}

void ARewindPlayerController::Jump()
{
	if (CharacterRewindComponent->IsTimeBeingManipulated() || RewindCharacter->bIsDead) { return; }
	if (RewindCharacter)
	{
		RewindCharacter->Jump();
	}
}

void ARewindPlayerController::StopJumping()
{
	if (CharacterRewindComponent->IsTimeBeingManipulated() || RewindCharacter->bIsDead) { return; }
	if (RewindCharacter)
	{
		RewindCharacter->StopJumping();
	}
}

void ARewindPlayerController::Rewind()
{
	check(GameMode);
	GameMode->StartGlobalRewind();
}

void ARewindPlayerController::StopRewind()
{
	check(GameMode);
	GameMode->StopGlobalRewind();
}

void ARewindPlayerController::FastForward()
{
	check(GameMode);
	GameMode->StartGlobalFastForward();
}

void ARewindPlayerController::StopFastForward()
{
	check(GameMode);
	GameMode->StopGlobalFastForward();
}

void ARewindPlayerController::ToggleTimeScrub()
{
	check(GameMode);
	GameMode->ToggleTimeScrub();
}

void ARewindPlayerController::SetRewindSpeedSlowest()
{
	check(GameMode);
	GameMode->SetRewindSpeedSlowest();
}

void ARewindPlayerController::SetRewindSpeedSlower()
{
	check(GameMode);
	GameMode->SetRewindSpeedSlower();
}

void ARewindPlayerController::SetRewindSpeedNormal()
{
	check(GameMode);
	GameMode->SetRewindSpeedNormal();
}

void ARewindPlayerController::SetRewindSpeedFaster()
{
	check(GameMode);
	GameMode->SetRewindSpeedFaster();
}

void ARewindPlayerController::SetRewindSpeedFastest()
{
	check(GameMode);
	GameMode->SetRewindSpeedFastest();
}

void ARewindPlayerController::ToggleRewindParticipation()
{
	RewindCharacter->GetCharacterRewindComponent()->SetIsRewindingEnabled(!RewindCharacter->GetCharacterRewindComponent()->IsRewindingEnabled());
}
