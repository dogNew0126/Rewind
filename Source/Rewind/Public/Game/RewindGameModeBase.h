// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "RewindGameModeBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGlobalRewindStarted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGlobalRewindCompleted);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGlobalFastForwardStarted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGlobalFastForwardCompleted);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGlobalTimeScrubStarted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGlobalTimeScrubCompleted);

UCLASS(minimalapi)
class ARewindGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	ARewindGameModeBase();

	UFUNCTION(BlueprintCallable, Category = "Rewind")
	void StartGlobalRewind();

	UFUNCTION(BlueprintCallable, Category = "Rewind")
	void StopGlobalRewind();

	UFUNCTION(BlueprintCallable, Category = "Rewind")
	void StartGlobalFastForward();

	UFUNCTION(BlueprintCallable, Category = "Rewind")
	void StopGlobalFastForward();

	UFUNCTION(BlueprintCallable, Category = "Rewind")
	void ToggleTimeScrub();
	FORCEINLINE void SetRewindSpeedSlowest() { GlobalRewindSpeed = SlowestRewindSpeed; };
	FORCEINLINE void SetRewindSpeedSlower() { GlobalRewindSpeed = SlowerRewindSpeed; };
	FORCEINLINE void SetRewindSpeedNormal() { GlobalRewindSpeed = NormalRewindSpeed; };
	FORCEINLINE void SetRewindSpeedFaster() { GlobalRewindSpeed = FasterRewindSpeed; };
	FORCEINLINE void SetRewindSpeedFastest() { GlobalRewindSpeed = FastestRewindSpeed; };

	UFUNCTION(BlueprintCallable, Category = "Rewind")
	FORCEINLINE float GetGlobalRewindSpeed() const { return GlobalRewindSpeed; }

	UFUNCTION(BlueprintCallable, Category = "Rewind")
	FORCEINLINE bool IsGlobalRewinding() const { return bIsGlobalRewinding; };
	UFUNCTION(BlueprintCallable, Category = "Rewind")
	FORCEINLINE bool IsGlobalFastForwarding() const { return bIsGlobalFastForwarding; };
	UFUNCTION(BlueprintCallable, Category = "Rewind")
	FORCEINLINE bool IsGlobalTimeScrubbing() const { return bIsGlobalTimeScrubbing; };
	UFUNCTION(BlueprintCallable, Category = "Rewind")
	FORCEINLINE bool IsTimeBeingManipulated() const { return bIsGlobalRewinding || bIsGlobalFastForwarding || bIsGlobalTimeScrubbing; };

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Total")
	int TotalPickableItemCount = 10;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Total")
	float TotalRewindSeconds = 300.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Total")
	int TotalEnemyCount = 10;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Total")
	int TotalRoomCount = 50;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rewind")
	float SlowestRewindSpeed = 0.25f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rewind")
	float SlowerRewindSpeed = 0.5f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rewind")
	float NormalRewindSpeed = 1.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rewind")
	float FasterRewindSpeed = 2.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rewind")
	float FastestRewindSpeed = 4.0f;

	UPROPERTY(BlueprintAssignable, Category = "Rewind")
	FOnGlobalRewindStarted OnGlobalRewindStarted;
	UPROPERTY(BlueprintAssignable, Category = "Rewind")
	FOnGlobalRewindCompleted OnGlobalRewindCompleted;

	UPROPERTY(BlueprintAssignable, Category = "Rewind")
	FOnGlobalFastForwardStarted OnGlobalFastForwardStarted;
	UPROPERTY(BlueprintAssignable, Category = "Rewind")
	FOnGlobalFastForwardCompleted OnGlobalFastForwardCompleted;

	UPROPERTY(BlueprintAssignable, Category = "Rewind")
	FOnGlobalTimeScrubStarted OnGlobalTimeScrubStarted;
	UPROPERTY(BlueprintAssignable, Category = "Rewind")
	FOnGlobalTimeScrubCompleted OnGlobalTimeScrubCompleted;

	// Desired length of longest rewind; used to compute the rewind buffer size
	UPROPERTY(EditDefaultsOnly, Category = "Rewind")
	float MaxRewindSeconds = 120.0f;

private:
	UPROPERTY(Transient, VisibleAnywhere, Category = "Rewind")
	float GlobalRewindSpeed = 1.0f;

	UPROPERTY(Transient, VisibleAnywhere, Category = "Rewind")
	bool bIsGlobalRewinding = false;

	UPROPERTY(Transient, VisibleAnywhere, Category = "Rewind")
	bool bIsGlobalFastForwarding = false;

	UPROPERTY(Transient, VisibleAnywhere, Category = "Rewind")
	bool bIsGlobalTimeScrubbing = false;
};



