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
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DeadlyTrap")
	USceneComponent* _rootScene = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DeadlyTrap")
	float _activationDuration = 3.f;
	
	float _timer;

	virtual void Activate();

	virtual void Deactivate();

	virtual void KillPlayer(AActor* victim);
	
};
