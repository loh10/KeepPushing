// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DeadlyTrap.h"
#include "GameFramework/Actor.h"
#include "SmasherTrap.generated.h"

UCLASS()
class KEEPPUSHING_API ASmasherTrap : public ADeadlyTrap
{
	GENERATED_BODY()

public:
	ASmasherTrap();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Smasher")
	UStaticMeshComponent* _leftBlock;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Smasher")
	UStaticMeshComponent* _rightBlock;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Smasher")
	float _moveSpeed = 2000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Smasher")
	float _direction = 1.f;
	
	bool _isClosing = false;

	FVector _leftStart;
	FVector _rightStart;

	
	virtual void Activate() override;
	virtual void Deactivate() override;

	UFUNCTION()
	void OnComponentHit(
			UPrimitiveComponent* hitComponent,
			AActor* otherActor,
			UPrimitiveComponent* otherComp,
			FVector normalImpulse,
			const FHitResult& hit
		);

	virtual void KillPlayer(AActor* victim) override;

private:
	static bool CheckVectorEpsilon(const FVector& start, const FVector& target, float epsilon = 0.01f);
};
