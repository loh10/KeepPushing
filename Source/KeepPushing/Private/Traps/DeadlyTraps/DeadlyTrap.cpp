#include "Traps/DeadlyTraps/DeadlyTrap.h"


ADeadlyTrap::ADeadlyTrap()
{
	PrimaryActorTick.bCanEverTick = true;

	_rootScene = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
	_rootScene->SetupAttachment(RootComponent);
}

void ADeadlyTrap::BeginPlay()
{
	Super::BeginPlay();
	_timer = _activationDuration;
}

void ADeadlyTrap::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	_timer -= DeltaTime;
	
	if (_timer <= 0.f)
	{
		Activate();
		return;
	}
}

void ADeadlyTrap::Activate()
{
	//UE_LOG(LogTemp, Warning, TEXT("_isActivated = %s"), _isActivated ? TEXT("true") : TEXT("false"));
	// Child can override to add visuals/audio
	// After activation duration, go back to arming
}

void ADeadlyTrap::Deactivate()
{
	// Child can override to add visuals/audio
	// After arming duration, activate again
}

void ADeadlyTrap::KillPlayer(AActor* victim)
{
	
}
