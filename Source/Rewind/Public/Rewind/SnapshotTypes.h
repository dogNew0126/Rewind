

#pragma once

#include "SnapshotTypes.generated.h"

USTRUCT(BlueprintType)
struct FBaseSnapshot
{
	GENERATED_BODY()

	// Time since the last snapshot was recorded
	UPROPERTY(Transient)
	float TimeSinceLastSnapshot = 0.0f;

	// Transform at time snapshot was recorded
	UPROPERTY(Transient)
	FTransform Transform{ FVector::ZeroVector };

	// Linear velocity from the owner's root primitive component at time snapshot was recorded
	UPROPERTY(Transient)
	FVector LinearVelocity = FVector::ZeroVector;

	// Angular velocity from the owner's root primitive component at time snapshot was recorded
	UPROPERTY(Transient)
	FVector AngularVelocityInRadians = FVector::ZeroVector;
};

USTRUCT(BlueprintType)
struct FPlayerSnapshot
{
	GENERATED_BODY()

	// Time since the last snapshot was recorded
	UPROPERTY(Transient)
	float TimeSinceLastSnapshot = 0.0f;

	// Movement velocity from the owner's movement component at time snapshot was recorded
	UPROPERTY(Transient)
	FVector MovementVelocity = FVector::ZeroVector;

	// Movement mode from the owner's movement component at time snapshot was recorded
	UPROPERTY(Transient)
	TEnumAsByte<enum EMovementMode> MovementMode = EMovementMode::MOVE_None;
};
