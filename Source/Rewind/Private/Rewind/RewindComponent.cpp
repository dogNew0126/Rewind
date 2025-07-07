// Fill out your copyright notice in the Description page of Project Settings.


#include "Rewind/RewindComponent.h"
#include "Game/RewindGameModeBase.h"


URewindComponent::URewindComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	// Tick after movement is completed
	PrimaryComponentTick.TickGroup = TG_PostPhysics;
}

void URewindComponent::SetIsRewindingEnabled(bool bEnabled)
{
	bIsRewindingEnabled = bEnabled;
	if (!bIsRewindingEnabled)
	{
		// Stop time manipulation if necessary
		if (bIsRewinding) { OnGlobalRewindCompleted(); }
		if (bIsFastForwarding) { OnGlobalFastForwardCompleted(); }
		if (bIsTimeScrubbing) { OnGlobalTimeScrubCompleted(); }
	}
	else
	{
		check(GameMode);

		// Start manipulation if global manipulation is in progress
		if (!bIsRewinding && GameMode->IsGlobalRewinding()) { OnGlobalRewindStarted(); }
		if (!bIsFastForwarding && GameMode->IsGlobalFastForwarding()) { OnGlobalFastForwardStarted(); }
		if (!bIsTimeScrubbing && GameMode->IsGlobalTimeScrubbing()) { OnGlobalTimeScrubStarted(); }
	}
}

void URewindComponent::BeginPlay()
{
	Super::BeginPlay();

	GameMode = Cast<ARewindGameModeBase>(GetWorld()->GetAuthGameMode());

	if (!GameMode)
	{
		SetComponentTickEnabled(false);
		return;
	}

	OwnerRootComponent = Cast<UPrimitiveComponent>(GetOwner()->GetRootComponent());

	GameMode->OnGlobalRewindStarted.AddUniqueDynamic(this, &URewindComponent::OnGlobalRewindStarted);
	GameMode->OnGlobalRewindCompleted.AddUniqueDynamic(this, &URewindComponent::OnGlobalRewindCompleted);
	GameMode->OnGlobalFastForwardStarted.AddUniqueDynamic(this, &URewindComponent::OnGlobalFastForwardStarted);
	GameMode->OnGlobalFastForwardCompleted.AddUniqueDynamic(this, &URewindComponent::OnGlobalFastForwardCompleted);
	GameMode->OnGlobalTimeScrubStarted.AddUniqueDynamic(this, &URewindComponent::OnGlobalTimeScrubStarted);
	GameMode->OnGlobalTimeScrubCompleted.AddUniqueDynamic(this, &URewindComponent::OnGlobalTimeScrubCompleted);

	InitializeRingBuffers(GameMode->MaxRewindSeconds);
}

void URewindComponent::OnGlobalRewindStarted()
{
	bool bAlreadyManipulatingTime = IsTimeBeingManipulated();
	if (TryStartTimeManipulation(bIsRewinding, !bIsTimeScrubbing))
	{
		OnRewindStarted.Broadcast();
		if (!bAlreadyManipulatingTime) { OnTimeManipulationStarted.Broadcast(); }
	}
}

void URewindComponent::OnGlobalFastForwardStarted()
{
	bool bAlreadyManipulatingTime = IsTimeBeingManipulated();
	// Fast forward only if not time scrubbing
	if (bIsTimeScrubbing && TryStartTimeManipulation(bIsFastForwarding, !bIsTimeScrubbing))
	{
		OnFastForwardStarted.Broadcast();
		if (!bAlreadyManipulatingTime) { OnTimeManipulationStarted.Broadcast(); }
	}
}

void URewindComponent::OnGlobalTimeScrubStarted()
{
	bool bAlreadyManipulatingTime = IsTimeBeingManipulated();
	if (TryStartTimeManipulation(bIsTimeScrubbing, false))
	{
		OnTimeScrubStarted.Broadcast();
		if (!bAlreadyManipulatingTime) { OnTimeManipulationStarted.Broadcast(); }
	}
}

void URewindComponent::OnGlobalRewindCompleted()
{
	if (TryStopTimeManipulation(bIsRewinding, !bIsTimeScrubbing))
	{
		// Mark last operation as a rewind for subsequent time scrubbing
		bLastTimeManipulationWasRewind = true;

		OnRewindCompleted.Broadcast();
		if (!IsTimeBeingManipulated()) { OnTimeManipulationCompleted.Broadcast(); }
	}
}

void URewindComponent::OnGlobalFastForwardCompleted()
{
	if (TryStopTimeManipulation(bIsFastForwarding, !bIsTimeScrubbing))
	{
		// Mark last operation as a fast forward for subsequent time scrubbing
		bLastTimeManipulationWasRewind = false;

		OnFastForwardCompleted.Broadcast();
		if (!IsTimeBeingManipulated()) { OnTimeManipulationCompleted.Broadcast(); }
	}
}

void URewindComponent::OnGlobalTimeScrubCompleted()
{
	if (TryStopTimeManipulation(bIsTimeScrubbing, false /*bResetTimeSinceSnapshotsChanged*/))
	{
		OnFastForwardCompleted.Broadcast();
		if (!IsTimeBeingManipulated()) { OnTimeManipulationCompleted.Broadcast(); }
	}
}

bool URewindComponent::TryStartTimeManipulation(bool& bStateToSet, bool bResetTimeSinceSnapshotsChanged)
{
	if (!bIsRewindingEnabled || bStateToSet) { return false; }

	bStateToSet = true;
	if (bResetTimeSinceSnapshotsChanged) { TimeSinceSnapshotsChanged = 0.0f; }

	PausePhysics();
	bAnimationsPausedAtStartOfTimeManipulation = bPausedAnimation;

	return true;
}

bool URewindComponent::TryStopTimeManipulation(bool& bStateToSet, bool bResetTimeSinceSnapshotsChanged)
{
	if (!bStateToSet) { return false; }

	// Turn off requested time manipulation (i.e. bIsRewinding, bIsFastForwarding, bIsTimeScrubbing)
	bStateToSet = false;

	if (!bIsTimeScrubbing)
	{
		if (bResetTimeSinceSnapshotsChanged) { TimeSinceSnapshotsChanged = 0.0f; }

		UnpausePhysics();
		UnpauseAnimation();

		if (LatestSnapshotIndex >= 0)
		{
			ApplySnapshotWhenStopTimeManipulation();
		}

		// Delete any future snapshots on the timeline that should be overwritten by new snapshots
		EraseFutureSnapshots();
	}

	return true;
}

void URewindComponent::PausePhysics()
{
	if (OwnerRootComponent && OwnerRootComponent->BodyInstance.bSimulatePhysics)
	{
		bPausedPhysics = true;
		OwnerRootComponent->SetSimulatePhysics(false);
	}
}

void URewindComponent::UnpausePhysics()
{
	if (!bPausedPhysics) { return; }

	check(OwnerRootComponent);
	bPausedPhysics = false;
	OwnerRootComponent->SetSimulatePhysics(true);
	OwnerRootComponent->RecreatePhysicsState();
}

void URewindComponent::PauseAnimation()
{
}

void URewindComponent::UnpauseAnimation()
{
}

void URewindComponent::PauseTime(float DeltaTime, bool bRewinding)
{
	if(HandleInsufficientSnapshots()) { return; }

	PauseAnimation();
	ApplySnapshotWhenPauseTimeOrEmptyBuffer();

	return;
}

void URewindComponent::InitializeRingBuffers(float MaxRewindSeconds)
{
	MaxSnapshots = FMath::CeilToInt(MaxRewindSeconds / SnapshotFrequencySeconds);

	uint32 SnapshotBytes = sizeof(FBaseSnapshot);
	uint32 TotalSnapshotBytes = MaxSnapshots * SnapshotBytes;

	ensureMsgf(
		TotalSnapshotBytes < MaxMemory,
		TEXT("Actor %s has rewind component that requested %d bytes of snapshots. Check snapshot frequency!"),
		*GetOwner()->GetName(),
		TotalSnapshotBytes);

	MaxSnapshots = FMath::Min(MaxSnapshots, static_cast<uint32>(MaxMemory / SnapshotBytes));

	Snapshots.Reserve(MaxSnapshots);

}

void URewindComponent::RecordSnapshot(float DeltaTime)
{
	TimeSinceSnapshotsChanged += DeltaTime;

	// Early out if last snapshot was taken within the desired snapshot cadence
	if (TimeSinceSnapshotsChanged < SnapshotFrequencySeconds && Snapshots.Num() != 0) { return; }

	// If the buffer is full, drop the oldest snapshot
	if (Snapshots.Num() == MaxSnapshots) { Snapshots.PopFront(); }

	OnRecordSnapshot();

	TimeSinceSnapshotsChanged = 0.0f;
}

void URewindComponent::OnRecordSnapshot()
{
	FTransform Transform = GetOwner()->GetActorTransform();
	FVector LinearVelocity = OwnerRootComponent ? OwnerRootComponent->GetPhysicsLinearVelocity() : FVector::Zero();
	FVector AngularVelocityInRadians = OwnerRootComponent ? OwnerRootComponent->GetPhysicsAngularVelocityInRadians() : FVector::Zero();
	LatestSnapshotIndex = Snapshots.Emplace(TimeSinceSnapshotsChanged, Transform, LinearVelocity, AngularVelocityInRadians);
}

void URewindComponent::EraseFutureSnapshots()
{
	while (LatestSnapshotIndex < Snapshots.Num() - 1)
	{
		Snapshots.Pop();
	}
}

void URewindComponent::PlaySnapshots(float DeltaTime, bool bRewinding)
{
	UnpauseAnimation();

	if (HandleInsufficientSnapshots()) { return; }

	// Apply time dilation to delta time
	DeltaTime *= GameMode->GetGlobalRewindSpeed();
	TimeSinceSnapshotsChanged += DeltaTime;

	bool bReachedEndOfTrack = false;
	float LatestSnapshotTime = Snapshots[LatestSnapshotIndex].TimeSinceLastSnapshot;
	if (bRewinding)
	{
		// Drop any snapshots that are too old to be relevant
		while (LatestSnapshotIndex > 0 && TimeSinceSnapshotsChanged > LatestSnapshotTime)
		{
			TimeSinceSnapshotsChanged -= LatestSnapshotTime;
			LatestSnapshotTime = Snapshots[LatestSnapshotIndex].TimeSinceLastSnapshot;
			--LatestSnapshotIndex;
		}

		// If we don't have any snapshots in the future, we can't interpolate, so just snap to the latest snapshot
		if (LatestSnapshotIndex == Snapshots.Num() - 1)
		{
			ApplySnapshotWhenPauseTimeOrEmptyBuffer();
			return;
		}

		bReachedEndOfTrack = LatestSnapshotIndex == 0;
	}
	else
	{
		// Drop any snapshots that are too old to be relevant
		while (LatestSnapshotIndex < Snapshots.Num() - 1 && TimeSinceSnapshotsChanged > LatestSnapshotTime)
		{
			TimeSinceSnapshotsChanged -= LatestSnapshotTime;
			LatestSnapshotTime = Snapshots[LatestSnapshotIndex].TimeSinceLastSnapshot;
			++LatestSnapshotIndex;
		}

		bReachedEndOfTrack = LatestSnapshotIndex == Snapshots.Num() - 1;
	}

	// If we've reached the end of our track, clamp the interpolation and repause animation
	if (bReachedEndOfTrack)
	{
		TimeSinceSnapshotsChanged = FMath::Min(TimeSinceSnapshotsChanged, LatestSnapshotTime);
		if (bAnimationsPausedAtStartOfTimeManipulation) { PauseAnimation(); }
	}

	InterpolateAndApplySnapshots(bRewinding);
}

void URewindComponent::InterpolateAndApplySnapshots(bool bRewinding)
{
	check(Snapshots.Num() >= 2);
	check(bRewinding && LatestSnapshotIndex < Snapshots.Num() - 1 || !bRewinding && LatestSnapshotIndex > 0);
	int PreviousIndex = bRewinding ? LatestSnapshotIndex + 1 : LatestSnapshotIndex - 1;

	const FBaseSnapshot& PreviousSnapshot = Snapshots[PreviousIndex];
	const FBaseSnapshot& NextSnapshot = Snapshots[LatestSnapshotIndex];
	ApplySnapshot(
		BlendSnapshots(PreviousSnapshot, NextSnapshot, TimeSinceSnapshotsChanged / NextSnapshot.TimeSinceLastSnapshot),
		false /*bApplyPhysics*/);
}

void URewindComponent::ApplySnapshotWhenPauseTimeOrEmptyBuffer()
{
	ApplySnapshot(Snapshots[LatestSnapshotIndex], false /*bApplyPhysics*/);
}

void URewindComponent::ApplySnapshotWhenStopTimeManipulation()
{
	ApplySnapshot(Snapshots[LatestSnapshotIndex], true /*bApplyPhysics*/);
}

bool URewindComponent::HandleInsufficientSnapshots()
{
	if (LatestSnapshotIndex < 0 || Snapshots.Num() == 0) { return true; }

	// If only one snapshot is available, snap to it
	if (Snapshots.Num() == 1)
	{
		ApplySnapshotWhenPauseTimeOrEmptyBuffer();
		return true;
	}

	check(LatestSnapshotIndex >= 0 && LatestSnapshotIndex < Snapshots.Num());
	return false;
}

FBaseSnapshot URewindComponent::BlendSnapshots(const FBaseSnapshot& A, const FBaseSnapshot& B, float Alpha)
{
	Alpha = FMath::Clamp(Alpha, 0.0f, 1.0f);
	FBaseSnapshot BlendedSnapshot;
	BlendedSnapshot.Transform.Blend(A.Transform, B.Transform, Alpha);
	BlendedSnapshot.LinearVelocity = FMath::Lerp(A.LinearVelocity, B.LinearVelocity, Alpha);
	BlendedSnapshot.AngularVelocityInRadians = FMath::Lerp(A.AngularVelocityInRadians, B.AngularVelocityInRadians, Alpha);
	return BlendedSnapshot;
}

void URewindComponent::ApplySnapshot(const FBaseSnapshot& Snapshot, bool bApplyPhysics)
{
	GetOwner()->SetActorTransform(Snapshot.Transform);
	if (OwnerRootComponent && bApplyPhysics)
	{
		OwnerRootComponent->SetPhysicsLinearVelocity(Snapshot.LinearVelocity);
		OwnerRootComponent->SetPhysicsAngularVelocityInRadians(Snapshot.AngularVelocityInRadians);
	}
}

void URewindComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bIsRewinding) { PlaySnapshots(DeltaTime, true /*bRewinding*/); }
	else if (bIsFastForwarding) { PlaySnapshots(DeltaTime, false /*bRewinding*/); }
	else if (bIsTimeScrubbing) { PauseTime(DeltaTime, bLastTimeManipulationWasRewind); }
	else { RecordSnapshot(DeltaTime); }
}

