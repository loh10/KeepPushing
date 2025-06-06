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
	
	UPROPERTY(EditAnywhere, Category = "SpikeTrap")
	UStaticMeshComponent* _mesh = nullptr;
	
	UPROPERTY(EditAnywhere, Category = "SpikeTrap")
	float _spikeLifetime = 2.f;

	UPROPERTY(EditAnywhere, Category = "SpikeTrap")
	float _distanceToSpawn = 100.f;

	UPROPERTY(EditAnywhere, Category = "SpikeTrap")
	TSubclassOf<class ASpikeComponent> _spikeClass;

	UPROPERTY(EditAnywhere, Category = "SpikeTrap")
	UCurveFloat* _movementCurve = nullptr;

	virtual void Activate() override;
};
