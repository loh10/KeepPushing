#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Traps/Trap.h"
#include "DeadlyTrap.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTrapKillPlayer, AActor*, KilledPlayer);

UCLASS(Abstract)
class KEEPPUSHING_API ADeadlyTrap : public ATrap
{
	GENERATED_BODY()

public:
	ADeadlyTrap();

	UPROPERTY(BlueprintAssignable, Category = "DeadlyTrap")
	FOnTrapKillPlayer OnTrapKillPlayer;

protected:
	virtual void BeginPlay() override;

	UPROPERTY()
	USceneComponent* _rootScene;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DeadlyTrap")
	float _armingDuration = 3.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DeadlyTrap")
	float _activationDuration = 1.f;
	

	bool _isArmed = true;

	bool _isActivated = false;

	FTimerHandle _phaseTimer;

	
	virtual void Activate();

	virtual void Deactivate();

	virtual void KillPlayer(AActor* victim);
	
};
