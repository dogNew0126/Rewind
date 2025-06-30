// Fill out your copyright notice in the Description page of Project Settings.


#include "RewindPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/Character.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

void ARewindPlayerController::BeginPlay()
{
	Super::BeginPlay();
	check(RewindMappingContext);

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(RewindMappingContext, 0);
	}
}

void ARewindPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent))
	{
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ARewindPlayerController::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ARewindPlayerController::StopJumping);

		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ARewindPlayerController::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ARewindPlayerController::Look);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void ARewindPlayerController::Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();
	const FRotator Rotation = GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
		ControlledPawn->AddMovementInput(ForwardDirection, MovementVector.Y);
		ControlledPawn->AddMovementInput(RightDirection, MovementVector.X);
	}
}

void ARewindPlayerController::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
		ControlledPawn->AddControllerYawInput(LookAxisVector.X);
		ControlledPawn->AddControllerPitchInput(LookAxisVector.Y);
	}
}

void ARewindPlayerController::Jump()
{
	if (ACharacter* RewindCharacter = Cast<ACharacter>(GetPawn()))
	{
		RewindCharacter->Jump();
	}
}

void ARewindPlayerController::StopJumping()
{
	if (ACharacter* RewindCharacter = Cast<ACharacter>(GetPawn()))
	{
		RewindCharacter->StopJumping();
	}
}
