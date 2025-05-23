#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "Trap.generated.h"

UCLASS(Abstract)
class KEEPPUSHING_API ATrap : public AActor
{
	GENERATED_BODY()

public:
	ATrap();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Trap")
	UBoxComponent* _trigger;
};
