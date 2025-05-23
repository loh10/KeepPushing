#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Traps/Trap.h"
#include "NotDeadlyTrap.generated.h"

UCLASS(Abstract)
class KEEPPUSHING_API ANotDeadlyTrap : public ATrap
{
	GENERATED_BODY()

public:
	ANotDeadlyTrap();

protected:
	virtual void AffectPlayer(AActor* player);
};
