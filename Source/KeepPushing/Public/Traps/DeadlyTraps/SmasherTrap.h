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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Smasher")
	UStaticMeshComponent* _leftBlock;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Smasher")
	UStaticMeshComponent* _rightBlock;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Smasher")
	float _moveSpeed = 20000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Smasher")
	float _direction = 1.f;
	
	FVector _leftStart;
	FVector _rightStart;
	
	FVector _leftStop;
	FVector _rightStop;

	bool _isActivated = false;
	
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
};
