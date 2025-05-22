#include "KeepPushing/Public/Platform/MovingPlatform.h"


AMovingPlatform::AMovingPlatform()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AMovingPlatform::BeginPlay()
{
	Super::BeginPlay();
}

void AMovingPlatform::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Move(DeltaTime);
}

void AMovingPlatform::Move(float DeltaTime)
{

	if (_movementSpeed <= 0.f) return; // Zero division protection

	// Cycle calculation based on speed (complete outward + return cycle)
	float cycleLength = 2.f * _offsetTarget;
	float moveCycleDuration = cycleLength / _movementSpeed;
	
	_moveTimer += DeltaTime;
	if (_moveTimer >= moveCycleDuration)
	{
		_moveTimer -= moveCycleDuration;  
	}

	float alpha = _moveTimer / moveCycleDuration;
	float sinAlpha = 0.5f * (1.f - FMath::Cos(alpha * 2 * PI));

	FVector targetPos = startPosition;
	switch (_axisToMove)
	{
	case EAxisMovement::X:
		targetPos.X += _offsetTarget;
		break;
	case EAxisMovement::Y:
		targetPos.Y += _offsetTarget;
		break;
	case EAxisMovement::Z:
		targetPos.Z += _offsetTarget;
		break;
	}

	FVector newLocation = FMath::Lerp(startPosition, targetPos, sinAlpha);
	SetActorLocation(newLocation, true); // true to activate sweep (collision)
}

