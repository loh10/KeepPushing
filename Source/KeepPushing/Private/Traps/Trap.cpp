#include "Traps/Trap.h"


ATrap::ATrap()
{
	PrimaryActorTick.bCanEverTick = true;

	_trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger"));
	RootComponent = _trigger;
}

void ATrap::BeginPlay()
{
	Super::BeginPlay();
}
