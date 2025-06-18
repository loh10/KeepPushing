#include "Traps/DeadlyTraps/Spike/SpikeTrap.h"

#include "Sound/SoundManager.h"
#include "Traps/DeadlyTraps/Spike/SpikeComponent.h"


ASpikeTrap::ASpikeTrap()
{
	PrimaryActorTick.bCanEverTick = true;
	
	_mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	//RootComponent = _mesh;
	///SetRootComponent(_mesh);
	_mesh->SetupAttachment(_rootScene);

	_mesh->SetGenerateOverlapEvents(true);
}

void ASpikeTrap::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASpikeTrap::Activate()
{
	Super::Activate();
	
	const FVector spawnLocation = GetActorLocation() + GetActorForwardVector() * _distanceToSpawn;
	const FRotator spawnRotation = GetActorRotation();

	const FActorSpawnParameters params;

	ASpikeComponent* spike = GetWorld()->SpawnActor<ASpikeComponent>(_spikeClass, spawnLocation, spawnRotation, params);
	if (spike)
	{
		spike->Init(GetActorForwardVector(), _movementCurve, _spikeLifetime);
	}

	_timer = _activationDuration;

	USoundManager::Get(this)->PlaySoundAtLocation("Spike_Activate", GetActorLocation());
}
