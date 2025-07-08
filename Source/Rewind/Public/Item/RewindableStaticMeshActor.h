// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "RewindableStaticMeshActor.generated.h"


class URewindComponent;

UCLASS()
class REWIND_API ARewindableStaticMeshActor : public AStaticMeshActor
{
	GENERATED_BODY()

public:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rewind")
	TObjectPtr<URewindComponent> RewindComponent;

	ARewindableStaticMeshActor();
};
