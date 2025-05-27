#include "Traps/Trap.h"


ATrap::ATrap()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ATrap::BeginPlay()
{
	Super::BeginPlay();
}
