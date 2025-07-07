// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Runtime/Core/Public/Containers/RingBuffer.h"
#include "Rewind/SnapshotTypes.h"
#include "RewindComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTimeManipulationStarted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTimeManipulationCompleted);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRewindStarted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRewindCompleted);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFastForwardStarted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFastForwardCompleted);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTimeScrubStarted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTimeScrubCompleted);

class ARewindGameModeBase;
class IRewindInterface;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class REWIND_API URewindComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	URewindComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "Rewind")
	FORCEINLINE bool IsRewinding() const { return bIsRewinding; };
	UFUNCTION(BlueprintCallable, Category = "Rewind")
	FORCEINLINE bool IsFastForwarding() const { return bIsFastForwarding; };
	UFUNCTION(BlueprintCallable, Category = "Rewind")
	FORCEINLINE bool IsTimeScrubbing() const { return bIsTimeScrubbing; };
	UFUNCTION(BlueprintCallable, Category = "Rewind")
	FORCEINLINE bool IsTimeBeingManipulated() const { return bIsRewinding || bIsFastForwarding || bIsTimeScrubbing; };

	UFUNCTION(BlueprintCallable, Category = "Rewind")
	FORCEINLINE bool IsRewindingEnabled() const { return bIsRewindingEnabled; }
	UFUNCTION(BlueprintCallable, Category = "Rewind")
	void SetIsRewindingEnabled(bool bEnabled);

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnGlobalRewindStarted();
	UFUNCTION()
	void OnGlobalFastForwardStarted();
	UFUNCTION()
	void OnGlobalTimeScrubStarted();
	UFUNCTION()
	void OnGlobalRewindCompleted();
	UFUNCTION()
	void OnGlobalFastForwardCompleted();
	UFUNCTION()
	void OnGlobalTimeScrubCompleted();

	void PausePhysics();
	void UnpausePhysics();
	virtual void PauseAnimation();
	virtual void UnpauseAnimation();

	/** Snapshots */
	virtual void InitializeRingBuffers(float MaxRewindSeconds);
	virtual void OnRecordSnapshot();
	virtual void EraseFutureSnapshots();
	virtual void InterpolateAndApplySnapshots(bool bRewinding);
	virtual void ApplySnapshotWhenPauseTimeOrEmptyBuffer();
	virtual void ApplySnapshotWhenStopTimeManipulation();

	// Helper function for PlaySnapshots/PauseTime that handle cases where there are insufficient snapshots to interpolate

private:

	// Helper to start a time manipulation operation
	bool TryStartTimeManipulation(bool& bStateToSet, const bool bResetTimeSinceSnapshotsChanged);
	// Helper to stop a time manipulation operation
	bool TryStopTimeManipulation(bool& bStateToSet, const bool bResetTimeSinceSnapshotsChanged);

	void PlaySnapshots(float DeltaTime, bool bRewinding);
	void PauseTime(float DeltaTime, bool bRewinding);
	void RecordSnapshot(float DeltaTime);
	bool HandleInsufficientSnapshots();

	FBaseSnapshot BlendSnapshots(const FBaseSnapshot& A, const FBaseSnapshot& B, float Alpha);
	void ApplySnapshot(const FBaseSnapshot& Snapshot, bool bApplyPhysics);

public:	
	UPROPERTY(EditDefaultsOnly, Category = "Rewind")
	float SnapshotFrequencySeconds = 1.0f / 30.0f;
		
	UPROPERTY(BlueprintAssignable, Category = "Rewind")
	FOnTimeManipulationStarted OnTimeManipulationStarted;
	UPROPERTY(BlueprintAssignable, Category = "Rewind")
	FOnTimeManipulationCompleted OnTimeManipulationCompleted;

	UPROPERTY(BlueprintAssignable, Category = "Rewind")
	FOnRewindStarted OnRewindStarted;
	UPROPERTY(BlueprintAssignable, Category = "Rewind")
	FOnRewindCompleted OnRewindCompleted;

	UPROPERTY(BlueprintAssignable, Category = "Rewind")
	FOnFastForwardStarted OnFastForwardStarted;
	UPROPERTY(BlueprintAssignable, Category = "Rewind")
	FOnFastForwardCompleted OnFastForwardCompleted;

	UPROPERTY(BlueprintAssignable, Category = "Rewind")
	FOnTimeScrubStarted OnTimeScrubStarted;
	UPROPERTY(BlueprintAssignable, Category = "Rewind")
	FOnTimeScrubCompleted OnTimeScrubCompleted;

protected:
	UPROPERTY(Transient, VisibleAnywhere, BlueprintReadOnly, Category = "Rewind")
	bool bIsRewinding = false;

	UPROPERTY(Transient, VisibleAnywhere, BlueprintReadOnly, Category = "Rewind")
	bool bIsFastForwarding = false;

	UPROPERTY(Transient, VisibleAnywhere, BlueprintReadOnly, Category = "Rewind")
	bool bIsTimeScrubbing = false;

	UPROPERTY(VisibleAnywhere, Category = "Rewind")
	bool bIsRewindingEnabled = true;

	UPROPERTY(Transient, VisibleAnywhere, Category = "Rewind|Debug")
	float TimeSinceSnapshotsChanged = 0.0f;

	UPROPERTY(Transient, VisibleAnywhere, Category = "Rewind|Debug")
	int32 LatestSnapshotIndex = -1;

	UPROPERTY(Transient, VisibleAnywhere, Category = "Rewind|Debug")
	uint32 MaxSnapshots = 1;

	UPROPERTY(Transient, VisibleAnywhere, Category = "Rewind|Debug")
	bool bPausedPhysics = false;

	UPROPERTY(Transient, VisibleAnywhere, Category = "Rewind|Debug")
	bool bPausedAnimation = false;

	uint32 MaxMemory = 1024 * 1024;

	UPROPERTY(Transient, VisibleAnywhere, Category = "Rewind|Debug")
	TObjectPtr<ARewindGameModeBase> GameMode;

private:
	TRingBuffer<FBaseSnapshot> Snapshots;

	bool bShouldClearTimeSinceSnapshotsChanged = false;

	// Whether animation was paused when the current time manipulation operation began
	UPROPERTY(Transient, VisibleAnywhere, Category = "Rewind|Debug")
	bool bAnimationsPausedAtStartOfTimeManipulation = false;

	// Whether last manipulation was rewind or fast forward; used by time scrubbing to determine interpolation direction
	UPROPERTY(Transient, VisibleAnywhere, Category = "Rewind|Debug")
	bool bLastTimeManipulationWasRewind = true;

	UPROPERTY(Transient, VisibleAnywhere, Category = "Rewind|Debug")
	UPrimitiveComponent* OwnerRootComponent;
};
