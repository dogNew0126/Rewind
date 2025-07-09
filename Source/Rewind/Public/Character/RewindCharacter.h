// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "RewindCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
class UCharacterRewindComponent;
struct FInputActionValue;

UCLASS(config=Game)
class ARewindCharacter : public ACharacter
{
	GENERATED_BODY()

public:

	ARewindCharacter();

	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	FORCEINLINE UCharacterRewindComponent* GetCharacterRewindComponent() const { return CharacterRewindComponent; }

protected:
	// TODO: Die in specific time
	UFUNCTION(BlueprintCallable)
	void Die();

	// TODO: Realive in specific time
	UFUNCTION(BlueprintCallable)
	void Realive();

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Dead")
	bool bIsDead = false;

private:
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rewind", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCharacterRewindComponent> CharacterRewindComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Dead", meta = (AllowPrivateAccess = "true"))
	float DeathStartTime = 0.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Dead")
	TObjectPtr<UAnimMontage> DeathMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Dead")
	TObjectPtr<UAnimMontage> RealiveMontage;
};

