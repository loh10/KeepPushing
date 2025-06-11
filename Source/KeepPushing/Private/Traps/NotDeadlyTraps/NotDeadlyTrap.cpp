#include "Traps/NotDeadlyTraps/NotDeadlyTrap.h"


ANotDeadlyTrap::ANotDeadlyTrap()
{
	PrimaryActorTick.bCanEverTick = true;

	_rootScene = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
	SetRootComponent(_rootScene);
}

void ANotDeadlyTrap::AffectPlayer(AActor* player)
{
	
}
