// Fill out your copyright notice in the Description page of Project Settings.


#include "Rewind/CharacterRewindComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Game/RewindGameModeBase.h"
#include "Character/BaseCharacter.h"

UCharacterRewindComponent::UCharacterRewindComponent() : Super()
{
	MaxMemory = 3 * 1024 * 1024;
}

void UCharacterRewindComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UCharacterRewindComponent::BeginPlay()
{
	Super::BeginPlay();

	RewindCharacter = Cast<ABaseCharacter>(GetOwner());
	OwnerMovementComponent = CastChecked<UCharacterMovementComponent>(RewindCharacter->GetMovementComponent());
	OwnerSkeletalMesh = CastChecked<USkeletalMeshComponent>(RewindCharacter->GetMesh());

}

void UCharacterRewindComponent::EraseFutureSnapshots()
{
	Super::EraseFutureSnapshots();

	while (LatestSnapshotIndex < PlayerSnapshots.Num() - 1)
	{
		PlayerSnapshots.Pop();
	}
}

void UCharacterRewindComponent::InterpolateAndApplySnapshots(bool bRewinding)
{
	Super::InterpolateAndApplySnapshots(bRewinding);

	int PreviousIndex = bRewinding ? LatestSnapshotIndex + 1 : LatestSnapshotIndex - 1;

	const FPlayerSnapshot& PreviousSnapshot = PlayerSnapshots[PreviousIndex];
	const FPlayerSnapshot& NextSnapshot = PlayerSnapshots[LatestSnapshotIndex];
	ApplySnapshot(
		BlendSnapshots(PreviousSnapshot, NextSnapshot, TimeSinceSnapshotsChanged / NextSnapshot.TimeSinceLastSnapshot),
		true /*bApplyTimeDilationToVelocity*/);
}

void UCharacterRewindComponent::PauseAnimation()
{
	check(OwnerSkeletalMesh);
	bPausedAnimation = true;
	OwnerSkeletalMesh->bPauseAnims = true;
}

void UCharacterRewindComponent::UnpauseAnimation()
{
	if (!bPausedAnimation) { return; }

	check(OwnerSkeletalMesh);
	bPausedAnimation = false;
	OwnerSkeletalMesh->bPauseAnims = false;
}

void UCharacterRewindComponent::InitializeRingBuffers(float MaxRewindSeconds)
{
	Super::InitializeRingBuffers(MaxRewindSeconds);

	uint32 SnapshotBytes = sizeof(FBaseSnapshot) + sizeof(FPlayerSnapshot);
	uint32 TotalSnapshotBytes = MaxSnapshots * SnapshotBytes;

	ensureMsgf(
		TotalSnapshotBytes < MaxMemory,
		TEXT("Actor %s has rewind component that requested %d bytes of snapshots. Check snapshot frequency!"),
		*GetOwner()->GetName(),
		TotalSnapshotBytes);

	MaxSnapshots = FMath::Min(MaxSnapshots, static_cast<uint32>(MaxMemory / SnapshotBytes));

	PlayerSnapshots.Reserve(MaxSnapshots);
}

void UCharacterRewindComponent::ApplySnapshotWhenPauseTimeOrEmptyBuffer()
{
	Super::ApplySnapshotWhenPauseTimeOrEmptyBuffer();

	ApplySnapshot(PlayerSnapshots[LatestSnapshotIndex], true /*bApplyTimeDilationToVelocity*/);
}

void UCharacterRewindComponent::ApplySnapshotWhenStopTimeManipulation()
{
	Super::ApplySnapshotWhenStopTimeManipulation();

	ApplySnapshot(PlayerSnapshots[LatestSnapshotIndex], false /*bApplyTimeDilationToVelocity*/);
}

void UCharacterRewindComponent::OnRecordSnapshot()
{
	Super::OnRecordSnapshot();

	if (PlayerSnapshots.Num() == MaxSnapshots) { PlayerSnapshots.PopFront(); }

	// Record the movement velocity and movement mode
	FVector MovementVelocity = OwnerMovementComponent->Velocity;
	TEnumAsByte<EMovementMode> MovementMode = OwnerMovementComponent->MovementMode;
	bool bIsDead = RewindCharacter->bIsDead;
	float DeathPosition = 0.f;
	if (bIsDead)
	{
		DeathPosition = RewindCharacter->GetMesh()->GetAnimInstance()->Montage_GetPosition(RewindCharacter->DeathMontage);
	}
	int32 LatestMovementSnapshotIndex = PlayerSnapshots.Emplace(TimeSinceSnapshotsChanged, MovementVelocity, MovementMode, bIsDead, DeathPosition);
	check(LatestSnapshotIndex == LatestMovementSnapshotIndex);
}

void UCharacterRewindComponent::ApplySnapshot(const FPlayerSnapshot& Snapshot, bool bApplyTimeDilationToVelocity)
{
	if (OwnerMovementComponent)
	{
		OwnerMovementComponent->Velocity =
			bApplyTimeDilationToVelocity ? Snapshot.MovementVelocity * GameMode->GetGlobalRewindSpeed() : Snapshot.MovementVelocity;
		OwnerMovementComponent->SetMovementMode(Snapshot.MovementMode);
		RewindCharacter->bIsDead = Snapshot.bIsDead;
		if (RewindCharacter->bIsDead)
		{
			TObjectPtr<UAnimInstance> AnimInstance = RewindCharacter->GetMesh()->GetAnimInstance();
			if (IsRewinding())
			{
				if(!AnimInstance->GetActiveInstanceForMontage(RewindCharacter->RealiveMontage)) { AnimInstance->Montage_Play(RewindCharacter->RealiveMontage); }
				UE_LOG(LogTemp, Warning, TEXT("DeathPosition: %f"), RewindCharacter->RealiveMontage->GetPlayLength() - Snapshot.DeathPosition);
				AnimInstance->Montage_SetPosition(RewindCharacter->RealiveMontage, RewindCharacter->RealiveMontage->GetPlayLength() - Snapshot.DeathPosition);
			}
			else
			{
				if (!AnimInstance->GetActiveInstanceForMontage(RewindCharacter->DeathMontage)) { AnimInstance->Montage_Play(RewindCharacter->DeathMontage); }
				AnimInstance->Montage_SetPosition(RewindCharacter->DeathMontage, Snapshot.DeathPosition);
			}
		}
	}
}

FPlayerSnapshot UCharacterRewindComponent::BlendSnapshots(const FPlayerSnapshot& A, const FPlayerSnapshot& B, float Alpha)
{
	Alpha = FMath::Clamp(Alpha, 0.0f, 1.0f);
	FPlayerSnapshot BlendedSnapshot;
	BlendedSnapshot.MovementVelocity = FMath::Lerp(A.MovementVelocity, B.MovementVelocity, Alpha);
	BlendedSnapshot.MovementMode = Alpha < 0.5f ? A.MovementMode : B.MovementMode;
	BlendedSnapshot.bIsDead = Alpha < 0.5f ? A.bIsDead : B.bIsDead;
	BlendedSnapshot.DeathPosition = FMath::Lerp(A.DeathPosition, B.DeathPosition, Alpha);
	return BlendedSnapshot;
}
