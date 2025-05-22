#include "KeepPushing/Public/Platform//Platform.h"


APlatform::APlatform()
{
	PrimaryActorTick.bCanEverTick = true;
}

void APlatform::BeginPlay()
{
	Super::BeginPlay();
}

void APlatform::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

