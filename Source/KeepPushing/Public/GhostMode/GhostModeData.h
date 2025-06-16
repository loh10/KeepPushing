#pragma once

#include "CoreMinimal.h"
#include "GhostModeData.generated.h"

USTRUCT()
struct FGhostModePoint
{
	GENERATED_BODY()
	FGhostModePoint()
		: Position(FVector::ZeroVector), Rotator(FRotator::ZeroRotator)
	{
	}
	explicit FGhostModePoint(const FTransform transform)
	{
		Position = transform.GetLocation();
		Rotator = FRotator(transform.GetRotation());
	}

	UPROPERTY(VisibleAnywhere)
	FVector Position;
	
	UPROPERTY(VisibleAnywhere)
	FRotator Rotator;
};

USTRUCT()
struct FTrace
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere)
	TArray<FGhostModePoint> Points;

	UPROPERTY(VisibleAnywhere)
	float Time;
};