#include "Traps/DeadlyTraps/Spike/SpikeTrap.h"


ASpikeTrap::ASpikeTrap()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ASpikeTrap::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASpikeTrap::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

