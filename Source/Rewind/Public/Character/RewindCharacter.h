// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Character/BaseCharacter.h"
#include "Logging/LogMacros.h"
#include "RewindCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
class UCharacterRewindComponent;
struct FInputActionValue;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAddPickable);

UCLASS(config=Game)
class ARewindCharacter : public ABaseCharacter
{
	GENERATED_BODY()

public:

	ARewindCharacter();

	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	FORCEINLINE int GetPickUpCount() const { return PickUpCount; }
	void AddPickUpCount();

public:
	
	UPROPERTY(BlueprintAssignable, Category = "Pickable")
	FOnAddPickable OnAddPickable;

private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pickable", meta = (AllowPrivateAccess = "true"))
	int PickUpCount = 0;

};

