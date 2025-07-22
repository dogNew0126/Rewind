// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Pickable/Ball.h"
#include "Character/RewindCharacter.h"
#include "Kismet/GameplayStatics.h"

void ABall::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	TObjectPtr<ARewindCharacter> RewindCharacter = Cast<ARewindCharacter>(OtherActor);
	if (RewindCharacter)
	{
		if (PickupSound)
		{
			UGameplayStatics::PlaySoundAtLocation(
				this,
				PickupSound,
				GetActorLocation()
			);
		}
		RewindCharacter->SetPickUpCount(RewindCharacter->GetPickUpCount() + 1);
		FString Message = FString::Printf(TEXT("Pickable Count: %d"), RewindCharacter->GetPickUpCount());
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, Message);
		Destroy();
	}
}
