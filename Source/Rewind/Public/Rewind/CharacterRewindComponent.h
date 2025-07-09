// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Rewind/RewindComponent.h"
#include "CharacterRewindComponent.generated.h"

class UCharacterMovementComponent;
class ARewindCharacter;

UCLASS()
class REWIND_API UCharacterRewindComponent : public URewindComponent
{
	GENERATED_BODY()

public:
	UCharacterRewindComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
protected:
	virtual void BeginPlay() override;

	virtual void EraseFutureSnapshots() override;
	virtual void InterpolateAndApplySnapshots(bool bRewinding) override;

	virtual void PauseAnimation() override;
	virtual void UnpauseAnimation() override;

	virtual void InitializeRingBuffers(float MaxRewindSeconds) override;
	virtual void ApplySnapshotWhenPauseTimeOrEmptyBuffer() override;
	virtual void ApplySnapshotWhenStopTimeManipulation() override;
	virtual void OnRecordSnapshot() override;

private:
	void ApplySnapshot(const FPlayerSnapshot& Snapshot, bool bApplyTimeDilationToVelocity);

	FPlayerSnapshot BlendSnapshots(const FPlayerSnapshot& A, const FPlayerSnapshot& B, float Alpha);
	
private:
	TRingBuffer<FPlayerSnapshot> PlayerSnapshots;

	UPROPERTY(Transient, VisibleAnywhere, Category = "Character")
	TObjectPtr<ARewindCharacter> RewindCharacter;

	UPROPERTY(Transient, VisibleAnywhere, Category = "Rewind|Debug")
	TObjectPtr<UCharacterMovementComponent> OwnerMovementComponent;

	UPROPERTY(Transient, VisibleAnywhere, Category = "Rewind|Debug")
	TObjectPtr<USkeletalMeshComponent> OwnerSkeletalMesh;
};
