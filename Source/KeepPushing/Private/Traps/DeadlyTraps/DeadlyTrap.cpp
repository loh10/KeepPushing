#include "Traps/DeadlyTraps/DeadlyTrap.h"


ADeadlyTrap::ADeadlyTrap()
{
	PrimaryActorTick.bCanEverTick = true;

	_rootScene = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
	RootComponent = _rootScene;
}

void ADeadlyTrap::BeginPlay()
{
	Super::BeginPlay();
}

void ADeadlyTrap::Activate()
{
	_isActivated = true;
	_isArmed = false;
	UE_LOG(LogTemp, Warning, TEXT("_isActivated = %s"), _isActivated ? TEXT("true") : TEXT("false"));
	// Child can override to add visuals/audio
	// After activation duration, go back to arming
}

void ADeadlyTrap::Deactivate()
{
	_isActivated = false;
	_isArmed = true;
	// Child can override to add visuals/audio
	// After arming duration, activate again
}

void ADeadlyTrap::KillPlayer(AActor* victim)
{
	
}
