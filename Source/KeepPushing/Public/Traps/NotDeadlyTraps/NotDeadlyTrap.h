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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DeadlyTrap")
	USceneComponent* _rootScene = nullptr;

	virtual void AffectPlayer(AActor* player);
};
