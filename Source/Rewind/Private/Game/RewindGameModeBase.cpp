// Copyright Epic Games, Inc. All Rights Reserved.

#include "Game/RewindGameModeBase.h"
#include "Character/RewindCharacter.h"
#include "UObject/ConstructorHelpers.h"

ARewindGameModeBase::ARewindGameModeBase()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}

void ARewindGameModeBase::StartGlobalRewind()
{
	bIsGlobalRewinding = true;
	OnGlobalRewindStarted.Broadcast();
}

void ARewindGameModeBase::StopGlobalRewind()
{
	bIsGlobalRewinding = false;
	OnGlobalRewindCompleted.Broadcast();
}

void ARewindGameModeBase::StartGlobalFastForward()
{
	bIsGlobalFastForwarding = true;
	OnGlobalFastForwardStarted.Broadcast();
}

void ARewindGameModeBase::StopGlobalFastForward()
{
	bIsGlobalFastForwarding = false;
	OnGlobalFastForwardCompleted.Broadcast();
}

void ARewindGameModeBase::ToggleTimeScrub()
{
	bIsGlobalTimeScrubbing = !bIsGlobalTimeScrubbing;
	if (bIsGlobalTimeScrubbing)
	{
		OnGlobalTimeScrubStarted.Broadcast();
	}
	else
	{
		OnGlobalTimeScrubCompleted.Broadcast();
	}
}
