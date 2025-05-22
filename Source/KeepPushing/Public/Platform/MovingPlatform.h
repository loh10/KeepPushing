#pragma once

#include "CoreMinimal.h"
#include "Platform.h"
#include "GameFramework/Actor.h"
#include "MovingPlatform.generated.h"








UCLASS()
class KEEPPUSHING_API AMovingPlatform : public APlatform
{
	GENERATED_BODY()

public:
	AMovingPlatform();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Moving")
	EAxisMovement _axisToMove = EAxisMovement::X;

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Moving")
	float _offsetTarget = 500.f; // travel distance

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Moving")
	float _movementSpeed = 100.f;

	
	float _direction = 1.f; // 1 or -1 for round trip
	
	
	float _moveTimer = 0.f;

	
	void Move(float deltaTime);
};
