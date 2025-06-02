#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Traps/DeadlyTraps/DeadlyTrap.h"
#include "SpikeTrap.generated.h"

UCLASS()
class KEEPPUSHING_API ASpikeTrap : public ADeadlyTrap
{
	GENERATED_BODY()

public:
	ASpikeTrap();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
};
