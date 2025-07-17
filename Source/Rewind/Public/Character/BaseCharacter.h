// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BaseCharacter.generated.h"

class UCharacterRewindComponent;

UCLASS()
class REWIND_API ABaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ABaseCharacter();

	FORCEINLINE UCharacterRewindComponent* GetCharacterRewindComponent() const { return CharacterRewindComponent; }

protected:
	UFUNCTION(BlueprintCallable)
	virtual void Die();

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Dead")
	bool bIsDead = false;

	UPROPERTY(EditDefaultsOnly, Category = "Dead")
	TObjectPtr<UAnimMontage> DeathMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Dead")
	TObjectPtr<UAnimMontage> RealiveMontage;
private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rewind", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCharacterRewindComponent> CharacterRewindComponent;
};
