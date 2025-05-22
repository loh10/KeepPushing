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

	if (movementSpeed <= 0.f) return; // Protection division par zéro

	// Calcul du cycle selon la vitesse
	float cycleLength = 2.f * offsetTarget; // aller + retour
	float moveCycleDuration = cycleLength / movementSpeed;
	
	moveTimer += DeltaTime;
	if (moveTimer >= moveCycleDuration)
	{
		moveTimer -= moveCycleDuration;  
	}

	float alpha = moveTimer / moveCycleDuration;
	float sinAlpha = 0.5f * (1.f - FMath::Cos(alpha * 2 * PI));

	FVector targetPos = startPosition;
	switch (axisToMove)
	{
	case EAxisMovement::X:
		targetPos.X += offsetTarget;
		break;
	case EAxisMovement::Y:
		targetPos.Y += offsetTarget;
		break;
	case EAxisMovement::Z:
		targetPos.Z += offsetTarget;
		break;
	}

	FVector newLocation = FMath::Lerp(startPosition, targetPos, sinAlpha);
	SetActorLocation(newLocation, true); // true pour activer le sweep (collision)
}

