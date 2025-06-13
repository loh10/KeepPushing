#pragma once

#include "CoreMinimal.h"
#include "NotDeadlyTrap.h"
#include "GameFramework/Actor.h"
#include "JumperTrap2.generated.h"

UCLASS()
class KEEPPUSHING_API AJumperTrap2 : public ANotDeadlyTrap
{
	GENERATED_BODY()

public:
	AJumperTrap2();

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "JumperTrap")
	UBoxComponent* _trigger;
	
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "JumperTrap")
	float _jumpForceStart = 200.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "JumperTrap")
	float _jumpForceEnd = 2000.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "JumperTrap")
	float _ascendDuration = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "JumperTrap")
	float _oscillateDuration = 2.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "JumperTrap")
	float _descendDuration = 1.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "JumperTrap")
	float _heightTarget = 3000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "JumperTrap")
	float _delayBeforeActivation = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "JumperTrap", meta = (ClampMin = "0", ClampMax = "100", UIMin = "0", UIMax = "100"))
	float _reduceSpeedPercent = 90.f;
	
	bool _isActive = false;
	bool _isAscending = false;
	bool _isOscillating = false;

	float _animElapsed = 0.f;
	FTimerHandle _delayTimerHandle;

	AActor* _targetActor = nullptr;

	FVector _initialLocation;

	UFUNCTION()
	void ActivateTrap();
	
	UFUNCTION()
	virtual void AffectPlayer(AActor* player) override;

	UFUNCTION()
	void JumpPlayer(float jumpForce);


	UFUNCTION()
	void OnComponentBeginOverlap(
		UPrimitiveComponent* overlappedComponent,
		AActor* otherActor,
		UPrimitiveComponent* otherComp,
		int32 otherBodyIndex,
		bool bFromSweep,
		const FHitResult& sweepResult
	);
};
