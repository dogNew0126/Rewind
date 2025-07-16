// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/Pickable/PickableItem.h"
#include "Ball.generated.h"

/**
 * 
 */
UCLASS()
class REWIND_API ABall : public APickableItem
{
	GENERATED_BODY()

protected:

	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

private:
	UPROPERTY(EditAnywhere, Category = "Sound")
	TObjectPtr<USoundBase> PickupSound;
	
};
