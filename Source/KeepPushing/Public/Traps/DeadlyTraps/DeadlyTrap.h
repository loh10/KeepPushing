#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Traps/Trap.h"
#include "DeadlyTrap.generated.h"

UCLASS(Abstract)
class KEEPPUSHING_API ADeadlyTrap : public ATrap
{
	GENERATED_BODY()

public:
	ADeadlyTrap();

protected:
	virtual void KillPlayer(AActor* victim);
};
